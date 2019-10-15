#pragma once
#include <smmintrin.h>
#include <iostream>
#include <cstdlib>
#if __APPLE__
# include <stdlib.h>
#else
# include <malloc.h>
#endif


// __m128 bits mask to target the floating point sign bit.
static const __m128 SIGNMASK = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));

/**
** 16-bytes aligned memory allocation function.
** \param size Size of the memory chunk to allocate in bytes.
** \return A pointer to the newly aligned memory, or nullptr.
*/
void *malloc_simd(const size_t size);

/**
** 16-bytes aligned memory free function.
** \param v Memory pointer to free, which must have been allocated using
** malloc_simd.
*/
void free_simd(void *v);


/**
** 3D floating-point precission mathematical vector class.
*/
#ifdef __GNUC__
class __attribute__((aligned(16))) Vector3
#else
_MM_ALIGN16 class SimdVector3
#endif
{
public:
    /// Constructors
    inline SimdVector3() : mmvalue(_mm_setzero_ps()) {}
    inline SimdVector3(float x, float y, float z) : mmvalue(_mm_set_ps(0, z, y, x)) {}
    inline SimdVector3(__m128 m) : mmvalue(m) {}

    /// Arithmetic operators with Vector3
    inline SimdVector3 operator+(const SimdVector3 &b) const { return _mm_add_ps(mmvalue, b.mmvalue); }
    inline SimdVector3 operator-(const SimdVector3 &b) const { return _mm_sub_ps(mmvalue, b.mmvalue); }
    inline SimdVector3 operator*(const SimdVector3 &b) const { return _mm_mul_ps(mmvalue, b.mmvalue); }
    inline SimdVector3 operator/(const SimdVector3 &b) const { return _mm_div_ps(mmvalue, b.mmvalue); }

    /// Assignation and arithmetic operators with Vector3
    inline SimdVector3 &operator+=(const SimdVector3 &b) { mmvalue = _mm_add_ps(mmvalue, b.mmvalue); return *this; }
    inline SimdVector3 &operator-=(const SimdVector3 &b) { mmvalue = _mm_sub_ps(mmvalue, b.mmvalue); return *this; }
    inline SimdVector3 &operator*=(const SimdVector3 &b) { mmvalue = _mm_mul_ps(mmvalue, b.mmvalue); return *this; }
    inline SimdVector3 &operator/=(const SimdVector3 &b) { mmvalue = _mm_div_ps(mmvalue, b.mmvalue); return *this; }

    /// Arithmetic operators with floats
    inline SimdVector3 operator+(float b) const { return _mm_add_ps(mmvalue, _mm_set1_ps(b)); }
    inline SimdVector3 operator-(float b) const { return _mm_sub_ps(mmvalue, _mm_set1_ps(b)); }
    inline SimdVector3 operator*(float b) const { return _mm_mul_ps(mmvalue, _mm_set1_ps(b)); }
    inline SimdVector3 operator/(float b) const { return _mm_div_ps(mmvalue, _mm_set1_ps(b)); }

    /// Assignation and arithmetic operators with float
    inline SimdVector3 &operator+=(float b) { mmvalue = _mm_add_ps(mmvalue, _mm_set1_ps(b)); return *this; }
    inline SimdVector3 &operator-=(float b) { mmvalue = _mm_sub_ps(mmvalue, _mm_set1_ps(b)); return *this; }
    inline SimdVector3 &operator*=(float b) { mmvalue = _mm_mul_ps(mmvalue, _mm_set1_ps(b)); return *this; }
    inline SimdVector3 &operator/=(float b) { mmvalue = _mm_div_ps(mmvalue, _mm_set1_ps(b)); return *this; }

    /// Equality operators
    inline bool operator==(const SimdVector3 &b) const {
        return (((_mm_movemask_ps(_mm_cmpeq_ps(mmvalue, b.mmvalue))) & 0x7) == 0x7);
    }
    inline bool operator!=(const SimdVector3 &b) const { return !(*this == b); }

    /// Unary minus operator
    inline SimdVector3 operator-() const { return _mm_xor_ps(mmvalue, SIGNMASK); }

    /// Subscript operator
    // Note: there is not bound checking here.
    inline const float &operator[](const int i) const {
        return i == 0 ? this->x : (i == 1 ? this->y : this->z);
    }
    inline float &operator[](const int i) {
        return i == 0 ? this->x : (i == 1 ? this->y : this->z);
    }

    /// Cross product
    inline SimdVector3 cross(const SimdVector3 &b) const {
        return _mm_sub_ps(
            _mm_mul_ps(_mm_shuffle_ps(mmvalue, mmvalue, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(b.mmvalue, b.mmvalue, _MM_SHUFFLE(3, 1, 0, 2))),
            _mm_mul_ps(_mm_shuffle_ps(mmvalue, mmvalue, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(b.mmvalue, b.mmvalue, _MM_SHUFFLE(3, 0, 2, 1)))
        );
    }

    /// Dot product
    inline float dot(const SimdVector3 &b) const { return _mm_cvtss_f32(_mm_dp_ps(mmvalue, b.mmvalue, 0x71)); }
    /// Length of the vector
    inline float length() const { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(mmvalue, mmvalue, 0x71))); }
    /// Returns the normalized vector
    inline SimdVector3 normalize() const {
        // multiplying by rsqrt does not yield an accurate enough result, so we
        // divide by sqrt instead.
        return _mm_div_ps(mmvalue, _mm_sqrt_ps(_mm_dp_ps(mmvalue, mmvalue, 0xFF)));
    }

    /// Overloaded operators that ensure alignment
    inline void *operator new[](size_t x) { return malloc_simd(x); }
    inline void operator delete[](void *x) { if (x) free_simd(x); }

        /// Textual representation
    friend std::ostream &operator<<(std::ostream &os, const SimdVector3 &t);

    /// Direct access member variables.
    union {
        struct { float x, y, z; };
        __m128 mmvalue;
    };
};

/**
** External operators that maps to the actual Vector3 method.
*/
inline SimdVector3 operator+(float a, const SimdVector3 &b) { return b + a; }
inline SimdVector3 operator-(float a, const SimdVector3 &b) { return SimdVector3(_mm_set1_ps(a)) - b; }
inline SimdVector3 operator*(float a, const SimdVector3 &b) { return b * a; }
inline SimdVector3 operator/(float a, const SimdVector3 &b) { return SimdVector3(_mm_set1_ps(a)) / b; }