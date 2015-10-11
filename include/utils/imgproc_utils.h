#pragma once

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#include <vector>

template <typename T>
class SimpleBuf {
public:
	SimpleBuf() : buf_(0), size_(0) {}

	SimpleBuf(size_t size) : buf_(0) {
		resize(size);
	}

	~SimpleBuf() {
		if(buf_ != 0) {
			free(buf_);
		}
	}

	inline T &operator[](size_t index) {
		return buf_[index];
	}

	// Doesn't preserve contents.
	void resize(size_t size) {
		if(size_ < size) {
			if(buf_ != 0) {
				free(buf_);
			}
			buf_ = (T*)malloc(size * sizeof(T));
			size_ = size;
		}
	}

	T *data() {
		return buf_;
	}

	size_t size() const {
		return size_;
	}

private:
	T *buf_;
	size_t size_;
};

class TextureScaler {
public:
	TextureScaler();
	~TextureScaler();

	void Scale(u32* &data, int &width, int &height, int factor);

	enum { XBRZ = 0, HYBRID = 1, BICUBIC = 2, HYBRID_BICUBIC = 3 };

private:
	void ScaleXBRZ(int factor, u32* source, u32* dest, int width, int height);
	void ScaleBilinear(int factor, u32* source, u32* dest, int width, int height);
	void ScaleBicubicBSpline(int factor, u32* source, u32* dest, int width, int height);
	void ScaleBicubicMitchell(int factor, u32* source, u32* dest, int width, int height);
	void ScaleHybrid(int factor, u32* source, u32* dest, int width, int height, bool bicubic = false);

	void DePosterize(u32* source, u32* dest, int width, int height);

	bool IsEmptyOrFlat(u32* data, int pixels);

	// depending on the factor and texture sizes, these can get pretty large 
	// maximum is (100 MB total for a 512 by 512 texture with scaling factor 5 and hybrid scaling)
	// of course, scaling factor 5 is totally silly anyway
	SimpleBuf<u32> bufInput, bufDeposter, bufOutput, bufTmp1, bufTmp2, bufTmp3;
};

struct GlobalTexScaler {
	static TextureScaler& get() {
		static TextureScaler instance;
		return instance;
	}
};
