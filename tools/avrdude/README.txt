1. WinAVR에는 5.10버전이 포함되어 있으며, 이를 그대로 사용해도 무방함
2. JTAG ICE mk-II를 사용하여 프로그래밍을 하는 경우 
	2.1 avrstudio를 설치를 한 경우에는 jungo 드라이버가 사용되어 avrdude에서 usb 접근이 불가함
		이를 해결하기 위해 추가로 libusb32-filter 드라이버 설치가 필요
	2.2 avrstudio를 설치하지 않은 경우에는 WinAVR에 포함된 libusb32 native 드라이버를 이용하면 avrdude가 그냥 사용 가능 함


** JTAG ICE mk-II를 사용해 디버깅을 하기 위해선 AVR studio의 사용이 추천되므로 2.1 방식을 추천함.