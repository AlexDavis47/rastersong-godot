extends Control

@export var texture_rect: TextureRect
var dec = VideoDecoder.new()
var video_playing = false
var frame_count = 0
var current_frame_index = 0
var image = Image.new()
var texture = ImageTexture.new()

var fps = 30.0 # Target frames per second for playback
var frame_timer = 0.0

func _ready() -> void:
	# Test FFmpeg availability
	var test = FFmpegTester.new()
	print("=== FFmpeg Test Results ===")
	print("FFmpeg Version: ", test.get_ffmpeg_version())
	print("FFmpeg Available: ", test.is_ffmpeg_available())
	print("Libavcodec Version: ", test.get_libavcodec_version())
	print("")
	
	# Check if texture_rect is assigned
	if not texture_rect:
		print("ERROR: texture_rect not assigned in inspector!")
		return
	
	# Open video file
	var video_path = "C:\\Users\\boobo\\Desktop\\whiteruntzvid\\main.mp4"
	print("Opening video: ", video_path)
	
	if dec.open(video_path):
		print("Video opened successfully")
		print("Dimensions: ", dec.get_width(), "x", dec.get_height())
		video_playing = true
	else:
		print("ERROR: Failed to open video file")
		video_playing = false

func _process(delta: float) -> void:
	if not video_playing:
		return
	
	if frame_timer < 1.0 / fps:
		frame_timer += delta
		return
	else:
		frame_timer = 0.0

	# Decode the current frame
	var frame = dec.decode_frame(current_frame_index)
	if frame and frame.size() > 0:
		image.set_data(dec.get_width(), dec.get_height(), false, Image.FORMAT_RGBA8, frame)
		texture = ImageTexture.create_from_image(image)
		texture_rect.texture = texture
		frame_count += 1
		current_frame_index += 1
		
		# Print frame info periodically
		if frame_count % 30 == 0:
			print("Frames decoded: ", frame_count)
		if frame_count % 200 == 0:
			# Randomly seek to a different frame to test seeking
			current_frame_index = randi() % 1000
	else:
		# End of video reached
		print("Video playback finished. Total frames: ", frame_count)
		video_playing = false
		dec.close()

func _exit_tree() -> void:
	# Cleanup when scene is closed
	if dec.is_open():
		dec.close()
		print("Video decoder closed")
