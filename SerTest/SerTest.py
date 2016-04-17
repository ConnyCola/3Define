import os
import sys
import serial
import time

RED = 0xF800
BLUE = 0x00F8
GREEN = 0x07E0
BLACK = 0x0000


def send_pix(x, y, col, ser):
	ser.write(chr(x))
	ser.write(chr(y))
	ser.write(chr(col & 0xFF))
	ser.write(chr((col>>8)&0xFF))
	#print("send_pix(%i, %i, 0x%.4x)" % (x, y, col))
	#time.sleep(0.02)
	#s = ser.read(1)
	ser.readall
	#print s



def draw_rect(x0, y0 , x1 ,y1, col, ser):
	for i in range(x0, x1):
		for j in range(y0, y1):
			send_pix(i, j, col, ser)
			print("send_pix(%i, %i, 0x%.4x)" % (i, j, col))

def draw_rectL(x0, y0 , x1 ,y1, col, ser):
	for i in range(x0, x1, 2):
		for j in range(y0, y1):
			send_pix(i, j, col, ser)
			print("send_pix(%i, %i, 0x%.4x)" % (i, j, col))

def draw_rectR(x0, y0 , x1 ,y1, col, ser):
	for i in range(x0, x1, 2):
		for j in range(y0, y1):
			send_pix(i+1, j, col, ser)
			print("send_pix(%i, %i, 0x%.4x)" % (i+1, j, col))

def draw_rects(x, y, d, l, p, col, ser):
	for i in range(0, 3, 1):
		for j in range(0, 3, 1):
			draw_rectR((i*d)+x,(j*d)+y,(i*d)+l+x,(j*d)+y+l,col, ser)
			draw_rectL((i*d)+x+p,(j*d)+y,(i*d)+l+x+p,(j*d)+y+l, col, ser)

def draw_rects_sel(x, y, d, l, p, sx, sy, col, ser):
	for i in range(0, 3, 1):
		for j in range(0, 3, 1):
			if ((sx == i) and (sy == j)):
				draw_rectR((i*d)+x-(p/2),(j*d)+y,(i*d)+l+x-(p/2),(j*d)+y+l,col, ser)
				draw_rectL((i*d)+x+(p/2),(j*d)+y,(i*d)+l+x+(p/2),(j*d)+y+l, BLUE, ser)
			else:
				draw_rectR((i*d)+x,(j*d)+y,(i*d)+l+x,(j*d)+y+l,col, ser)
				draw_rectL((i*d)+x,(j*d)+y,(i*d)+l+x,(j*d)+y+l, col, ser)


if __name__ == "__main__":
	#ser = serial.Serial("/dev/tty.usbmodem14121", 57600)
	ser = serial.Serial("/dev/tty.usbmodem14111", 57600)
	#ser = serial.Serial("/dev/tty.usbmodem00000004", 57600)
	time.sleep(5)
	print("wait end")

	#draw_rectL(60, 20, 80, 40, GREEN, ser)
	#draw_rectR(64, 20, 84, 40, GREEN, ser)

	#draw_rectL(20, 20, 40, 40, GREEN, ser)
	#draw_rectR(24, 20, 44, 40, GREEN, ser)

	#draw_rects(20,20,30,20, 4, GREEN,ser)


	while True:
		draw_rects_sel(20,20,30,20, 12, 1, 1, GREEN, ser)



		time.sleep(7)
		draw_rects_sel(20,20,30,20, 12, 1, 0, GREEN, ser)


		time.sleep(3)
		draw_rects_sel(20,20,30,20, 12, 0, 2, GREEN, ser)

		time.sleep(3)
		draw_rects_sel(20,20,30,20, 12, 2, 2, GREEN, ser)

		time.sleep(2)
		draw_rects_sel(20,20,30,20, 12, 2, 1, GREEN, ser)
		time.sleep(2)
		draw_rects_sel(20,20,30,20, 12, 0, 1, GREEN, ser)
		time.sleep(2)
		draw_rects_sel(20,20,30,20, 12, 1, 2, GREEN, ser)
		time.sleep(2)
		draw_rects_sel(20,20,30,20, 12, 0, 0, GREEN, ser)
		time.sleep(2)
		draw_rects_sel(20,20,30,20, 12, 2, 0, GREEN, ser)
		time.sleep(2)

		draw_rect(0,0,128,128,BLACK,ser);


	#send_pix(20, 20, RED, ser)

	while(True):
		s = ser.read(1)
		sys.stdout.write(s)
