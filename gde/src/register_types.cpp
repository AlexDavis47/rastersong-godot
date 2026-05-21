#include "register_types.hpp"

#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "ffmpeg_tester.hpp"
#include "video_decoder.hpp"
#include "clip.hpp"
#include "video_clip.hpp"
#include "audio_clip.hpp"

using namespace godot;

void initialize_library_modules(ModuleInitializationLevel p_level)
{
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
  {
    return;
  }

  GDREGISTER_CLASS(FFmpegTester);
  GDREGISTER_CLASS(VideoDecoder);
  GDREGISTER_CLASS(Clip);
  GDREGISTER_CLASS(VideoClip);
  GDREGISTER_CLASS(AudioClip);
}

void unitialize_library_modules(ModuleInitializationLevel p_level)
{
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
  {
    return;
  }
}

extern "C"
{
  // Initialization.
  GDExtensionBool GDE_EXPORT
  library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
               GDExtensionClassLibraryPtr p_library,
               GDExtensionInitialization *r_initialization)
  {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library,
                                                   r_initialization);

    init_obj.register_initializer(initialize_library_modules);
    init_obj.register_terminator(unitialize_library_modules);
    init_obj.set_minimum_library_initialization_level(
        MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
  }
}
