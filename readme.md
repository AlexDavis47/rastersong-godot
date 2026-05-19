# Rastersong

A hybrid NLE/DAW built in Godot, where video is treated as a one-dimensional signal and processed through a DSP graph — the same way audio is processed in a DAW. Reverb, delay, compression, and any other signal processing effect can be applied to video, with results that are visually interesting precisely because DSP was never designed for 2D raster data.

---

## Core Concept

Video frames are flattened into a stream of pixels in raster-scan order. Each pixel is a four-channel float sample (RGBA, normalised 0–1). A frame is simply a contiguous block of `width × height` samples. This stream passes through a chain of processor nodes, then gets reassembled into frames for display or export.

The processor nodes have no concept of frames, seeks, caches, or playback state. They receive a flat buffer of samples and return a flat buffer of samples. All complexity around seeking, state warmup, and caching is handled by the engine around them — transparently.

---

## Architecture

The engine is split between a C++ GDExtension core and GDScript. The boundary is deliberate and strict: GDScript configures the graph and drives transport at human timescales (button clicks, parameter changes, one texture handoff per display frame). It never touches pixel data. C++ owns the entire execution loop.

### Modules

**Decoder** — wraps libavcodec. Translates frame-index seeks into raw RGBA float frames. Knows nothing about DSP.

**Decoded frame cache** — an LRU cache of raw decoded frames. Survives graph edits and seeks. Decoding is expensive; this ensures that tweaking an effect does not re-decode the same source footage.

**SampleStream** — presents the decoded cache as a flat, seekable sample stream. Handles the 2D-to-1D mapping and scan order. Everything downstream is insulated from how pixels were sourced.

**DSP Graph** — a chain of processor nodes. Accepts a flat buffer, routes it through the chain, returns the result. Runs on its own thread. Topology is set by GDScript once; execution is owned entirely by C++.

**Engine / Seek Coordinator** — sits between the DSP graph and the caches. Handles state warmup on seeks, manages the processed frame cache, and exposes the minimal API to GDScript.

**Processed frame cache** — an LRU cache of fully processed output frames. Keyed on both frame index and a graph version counter, so any parameter change or graph edit causes old entries to become unreachable and age out naturally — no explicit invalidation sweep needed.

**Frame Assembler** — converts processed sample buffers back into a Godot ImageTexture for display, or feeds the Encoder directly during offline render.

**Encoder** — wraps libavcodec write path. Offline render only.

**Transport / Clock** — tracks playhead position in sample units. Drives the DSP thread. Exposed to GDScript as play, pause, and seek.

---

## Processor Nodes

Every node is a black box that processes a flat buffer of pixel samples. A node declares two things about itself: how many samples of history it needs before its output becomes valid (pre-samples), and how many samples of future context it needs to produce correct output (post-samples).

A stateless per-pixel colour grade needs neither. A reverb node with a long impulse response needs pre-samples proportional to its tail. A lookahead limiter needs post-samples equal to its lookahead window. The node itself is completely unaware of how this information is used — it just processes whatever buffer it receives.

The graph takes the maximum pre and post requirements across all nodes. This is correct because all nodes process the same padded buffer simultaneously. A larger requirement covers a smaller one; requirements do not stack.

---

## State Warmup and Lookahead

This is the central problem the engine solves. Stateful processors accumulate history as they process samples. Seeking to an arbitrary frame leaves that history cold — the output will be wrong in a way that decays over time as history builds up, producing a visible artefact at the start of every seek.

The solution is simple: pad the input buffer with extra samples on each side, process the entire padded buffer as one flat stream, then crop the output back to exactly one frame of valid pixels. The processor never knows this happened.

When seeking cold to a given frame, the engine reaches back by the graph's pre-sample requirement and forward by the post-sample requirement, fetches that entire padded range, feeds it through the graph in one pass, and crops the result. The output is identical to what linear playback from the beginning would have produced.

During linear playback, pre-sample warmup cost collapses to zero because the processor state is already current. Only the post-sample tail needs to be fetched ahead. For a fully stateless graph, there is no padding at all and no crop — the overhead is exactly zero.

The engine tracks whether processor state is currently valid and where it last processed up to. On seek, state is marked cold and full padding is applied. After processing, state is marked warm. The processor never sees any of this tracking.

---

## Caching Strategy

Two independent LRU caches serve different purposes and have different invalidation lifecycles.

The **decoded frame cache** stores raw footage frames. It is keyed on clip identity and frame index. It survives seeks, graph edits, and parameter changes. Its purpose is to ensure that the expensive work of decoding is never repeated for a frame the engine has already decoded.

The **processed frame cache** stores fully processed output frames. It is keyed on frame index and a graph version counter. Any parameter change or graph edit increments the version counter, which makes all existing processed entries unreachable by their old keys. They age out of the LRU on their own without any sweep. Its purpose is to ensure that scrubbing and looping do not re-process frames the engine has already rendered.

The two caches are intentionally decoupled. When the user tweaks an effect, processed frames are implicitly invalidated but decoded frames are untouched. Re-processing starts immediately from raw data.

---

## Scan Order

The mapping from a 2D frame to a 1D sample stream is configurable. Different scan orders produce different visual characteristics when DSP is applied, because they determine which pixels are adjacent in the stream and therefore which pixels influence each other through stateful effects.

Raster order (left to right, top to bottom) is the default. DSP effects with memory smear horizontally within scanlines and vertically across rows. Serpentine order alternates row direction, reducing directional bias. Hilbert curve order uses a space-filling curve that preserves 2D locality in 1D, so neighbourhood-sensitive effects spread more isotropically across the image. Spiral order works from the centre out or the edge in.

Scan order is a property of the sample stream and is transparent to all processor nodes.

---

## Design Principles

**The processor is dumb by design.** Nodes know nothing about frames, seeks, caches, or warmup. This is a feature. It means any DSP algorithm can be dropped in as a node without modification, and correctness on seek is guaranteed by the engine regardless of what the node does internally.

**Frames are the unit of everything except processing.** Cache keys are frames. Seek targets are frames. The display is per-frame. The only place sub-frame granularity exists is inside the engine's padding arithmetic and inside individual processor implementations. This keeps the rest of the system simple.

**The GDScript boundary is crossed at human timescales.** GDScript touches the engine when the user does something — changes a parameter, clicks play, scrubs the timeline. It never participates in the sample processing loop. This makes the boundary safe to keep thin and cheap.

**Code is temporary, concepts are not.** This document describes the philosophy and data flow of the system. Implementation details live in the code.
