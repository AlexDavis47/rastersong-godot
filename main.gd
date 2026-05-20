extends Control

func _ready() -> void:
	var test = FFmpegTester.new()
	print(test.get_ffmpeg_version())
	print(test.is_ffmpeg_available())
	print(test.get_libavcodec_version())
