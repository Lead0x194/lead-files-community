PYTHON_SRCS		= metin2login.py  metin2packet.py
CIPHER_DIR		= ./cipher
all:
	$(MAKE) -C $(CIPHER_DIR) 
	-mkdir ../bin
	cp -f $(CIPHER_DIR)/*.so ../bin
	cp -f $(CIPHER_DIR)/*.py ../bin
	cp -f $(PYTHON_SRCS) ../bin

clean:
	$(MAKE) -C $(CIPHER_DIR) clean
