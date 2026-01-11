#include <cryptopp/cryptlib.h>

// Forward declaration
class KeyAgreement;

// Communication channel encryption handler.
class Cipher {
 public:
  Cipher();
  ~Cipher();

  void CleanUp();

  // Returns agreed value length in bytes, or zero on failure.
  size_t Prepare(char *buffer, int* length);
  // Try to activate cipher algorithm with agreement data received from peer.
  bool Activate(bool polarity, size_t agreed_length,
                const char *buffer, size_t length);

  // Encrypts the given block of data. (no padding required)
  void Encrypt(char *buffer, size_t length) {
    assert(activated_);
    if (!activated_) {
      return;
    }
    encoder_->ProcessData((byte*)buffer, (const byte*)buffer, length);
  }
  // Decrypts the given block of data. (no padding required)
  void Decrypt(char* buffer, size_t length) {
    assert(activated_);
    if (!activated_) {
      return;
    }
    decoder_->ProcessData((byte*)buffer, (const byte*)buffer, length);
  }

  bool activated() const { return activated_; }

  void set_activated(bool value) { activated_ = value; }
 private:
  bool SetUp(bool polarity);

  bool activated_;

  CryptoPP::SymmetricCipher* encoder_;
  CryptoPP::SymmetricCipher* decoder_;

  KeyAgreement* key_agreement_;
};

extern unsigned char get_sequence(unsigned int index);

