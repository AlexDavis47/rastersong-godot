extends Node

func _ready():
	var tester = FFmpegTester.new()
	print("FFmpeg Status:")
	print("  Available: ", tester.is_ffmpeg_available())
	print("  Message: ", tester.get_ffmpeg_version())
	print("  Versions:")
	print("    ", tester.get_libavutil_version())
	print("    ", tester.get_libavcodec_version())
	print("    ", tester.get_libavformat_version())
