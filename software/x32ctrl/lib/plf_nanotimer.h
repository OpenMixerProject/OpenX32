// Copyright (c) 2022, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgement in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.


#ifndef PLF_NANOTIMER_H
#define PLF_NANOTIMER_H


// Compiler-specific defines:

#define PLF_NOEXCEPT throw() // default before potential redefine

#if defined(_MSC_VER) && !defined(__clang__) && !defined(__GNUC__)
	#if _MSC_VER >= 1900
		#undef PLF_NOEXCEPT
		#define PLF_NOEXCEPT noexcept
	#endif
#elif defined(__cplusplus) && __cplusplus >= 201103L // C++11 support, at least
	#if defined(__GNUC__) && defined(__GNUC_MINOR__) && !defined(__clang__) // If compiler is GCC/G++
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4
			#undef PLF_NOEXCEPT
			#define PLF_NOEXCEPT noexcept
		#endif
	#elif defined(__clang__)
		#if __has_feature(cxx_noexcept)
			#undef PLF_NOEXCEPT
			#define PLF_NOEXCEPT noexcept
		#endif
	#else // Assume type traits and initializer support for other compilers and standard libraries
		#undef PLF_NOEXCEPT
		#define PLF_NOEXCEPT noexcept
	#endif
#endif



// ~Nanosecond-precision cross-platform (linux/bsd/mac/windows, C++03/C++11) simple timer class:

// Mac OSX implementation:
#if defined(__MACH__)
	#include <mach/clock.h>
	#include <mach/mach.h>

	namespace plf
	{

	class nanotimer
	{
	private:
		clock_serv_t system_clock;
		mach_timespec_t time1, time2;
	public:
		nanotimer() PLF_NOEXCEPT
		{
			host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &system_clock);
		}

		~nanotimer() PLF_NOEXCEPT
		{
			mach_port_deallocate(mach_task_self(), system_clock);
		}

		void start() PLF_NOEXCEPT
		{
			clock_get_time(system_clock, &time1);
		}

		double get_elapsed_ms() PLF_NOEXCEPT
		{
			return static_cast<double>(get_elapsed_ns()) / 1000000.0;
		}

		double get_elapsed_us() PLF_NOEXCEPT
		{
			return static_cast<double>(get_elapsed_ns()) / 1000.0;
		}

		double get_elapsed_ns() PLF_NOEXCEPT
		{
			clock_get_time(system_clock, &time2);
			return ((1000000000.0 * static_cast<double>(time2.tv_sec - time1.tv_sec)) + static_cast<double>(time2.tv_nsec - time1.tv_nsec));
		}
	};




// Linux/BSD implementation:
#elif (defined(linux) || defined(__linux__) || defined(__linux)) || (defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__))
	#include <time.h>
	#include <sys/time.h>

	namespace plf
	{

	class nanotimer
	{
	private:
		struct timespec time1, time2;
	public:
		nanotimer() PLF_NOEXCEPT {}

		void start() PLF_NOEXCEPT
		{
			clock_gettime(CLOCK_MONOTONIC, &time1);
		}

		double get_elapsed_ms() PLF_NOEXCEPT
		{
			return get_elapsed_ns() / 1000000.0;
		}

		double get_elapsed_us() PLF_NOEXCEPT
		{
			return get_elapsed_ns() / 1000.0;
		}

		double get_elapsed_ns() PLF_NOEXCEPT
		{
			clock_gettime(CLOCK_MONOTONIC, &time2);
			return ((1000000000.0 * static_cast<double>(time2.tv_sec - time1.tv_sec)) + static_cast<double>(time2.tv_nsec - time1.tv_nsec));
		}
	};




// Windows implementation:
#elif defined(_WIN32)
	#if defined(_MSC_VER) && !defined(__clang__) && !defined(__GNUC__) && !defined(NOMINMAX)
		#define NOMINMAX // Otherwise MS compilers act like idiots when using std::numeric_limits<>::max() and including windows.h
	#endif

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
		#undef WIN32_LEAN_AND_MEAN
	#else
		#include <windows.h>
	#endif

	namespace plf
	{

	class nanotimer
	{
	private:
		LARGE_INTEGER ticks1, ticks2;
		double frequency;
	public:
		nanotimer() PLF_NOEXCEPT
		{
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			frequency = static_cast<double>(freq.QuadPart);
		}

		void start() PLF_NOEXCEPT
		{
			QueryPerformanceCounter(&ticks1);
		}

		double get_elapsed_ms() PLF_NOEXCEPT
		{
			QueryPerformanceCounter(&ticks2);
			return (static_cast<double>(ticks2.QuadPart - ticks1.QuadPart) * 1000.0) / frequency;
		}

		double get_elapsed_us() PLF_NOEXCEPT
		{
			return get_elapsed_ms() * 1000.0;
		}

		double get_elapsed_ns() PLF_NOEXCEPT
		{
			return get_elapsed_ms() * 1000000.0;
		}
	};
#endif
// Else: failure warning - your OS is not supported



#if defined(__MACH__) || (defined(linux) || defined(__linux__) || defined(__linux)) || (defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)) || defined(_WIN32)
inline void nanosecond_delay(const double delay_ns) PLF_NOEXCEPT
{
	nanotimer timer;
	timer.start();

	while(timer.get_elapsed_ns() < delay_ns)
	{};
}


inline void microsecond_delay(const double delay_us) PLF_NOEXCEPT
{
	nanosecond_delay(delay_us * 1000.0);
}


inline void millisecond_delay(const double delay_ms) PLF_NOEXCEPT
{
	nanosecond_delay(delay_ms * 1000000.0);
}


} // namespace
#endif

#undef PLF_NOEXCEPT

#endif // PLF_NANOTIMER_H

