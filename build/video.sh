apitrace dump-images -o - tzcad.trace \
	| ffmpeg -r 30 -f image2pipe -vcodec ppm -i pipe: -r 30000/1001 -b 2M -bt 4M -vcodec libx264 -y tzcad.mp4
