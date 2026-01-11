from ctypes import cdll
from ctypes import create_string_buffer

BLOCK_SIZE = 1024

def conv(srcText, srcCodePage, dstCodePage):
	dstBufs = []

	dstBuf = create_string_buffer(BLOCK_SIZE * 4)

	srcPos = 0
	srcMax = len(srcText)

	while srcPos < srcMax:
		srcBlock = srcText[srcPos:srcPos+BLOCK_SIZE]
		dstLen = cdll.yconv.conv(srcCodePage, srcBlock, len(srcBlock), dstCodePage, dstBuf, len(dstBuf))
		dstBufs.append(dstBuf[:dstLen])
		srcPos += len(srcBlock)

	return "".join(dstBufs)
