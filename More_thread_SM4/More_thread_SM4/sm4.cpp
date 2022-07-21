#include<stdio.h>
#include<thread>
#include<iostream>
#include <ctime>
#include<windows.h>
#include <cstdlib>
#define b8 unsigned char
#define b32 unsigned long
#define thread_num 4

using namespace std;


b8 Sbox[256] = {
	0xd6,0x90,0xe9,0xfe,0xcc,0xe1,0x3d,0xb7,0x16,0xb6,0x14,0xc2,0x28,0xfb,0x2c,0x05,
	0x2b,0x67,0x9a,0x76,0x2a,0xbe,0x04,0xc3,0xaa,0x44,0x13,0x26,0x49,0x86,0x06,0x99,
	0x9c,0x42,0x50,0xf4,0x91,0xef,0x98,0x7a,0x33,0x54,0x0b,0x43,0xed,0xcf,0xac,0x62,
	0xe4,0xb3,0x1c,0xa9,0xc9,0x08,0xe8,0x95,0x80,0xdf,0x94,0xfa,0x75,0x8f,0x3f,0xa6,
	0x47,0x07,0xa7,0xfc,0xf3,0x73,0x17,0xba,0x83,0x59,0x3c,0x19,0xe6,0x85,0x4f,0xa8,
	0x68,0x6b,0x81,0xb2,0x71,0x64,0xda,0x8b,0xf8,0xeb,0x0f,0x4b,0x70,0x56,0x9d,0x35,
	0x1e,0x24,0x0e,0x5e,0x63,0x58,0xd1,0xa2,0x25,0x22,0x7c,0x3b,0x01,0x21,0x78,0x87,
	0xd4,0x00,0x46,0x57,0x9f,0xd3,0x27,0x52,0x4c,0x36,0x02,0xe7,0xa0,0xc4,0xc8,0x9e,
	0xea,0xbf,0x8a,0xd2,0x40,0xc7,0x38,0xb5,0xa3,0xf7,0xf2,0xce,0xf9,0x61,0x15,0xa1,
	0xe0,0xae,0x5d,0xa4,0x9b,0x34,0x1a,0x55,0xad,0x93,0x32,0x30,0xf5,0x8c,0xb1,0xe3,
	0x1d,0xf6,0xe2,0x2e,0x82,0x66,0xca,0x60,0xc0,0x29,0x23,0xab,0x0d,0x53,0x4e,0x6f,
	0xd5,0xdb,0x37,0x45,0xde,0xfd,0x8e,0x2f,0x03,0xff,0x6a,0x72,0x6d,0x6c,0x5b,0x51,
	0x8d,0x1b,0xaf,0x92,0xbb,0xdd,0xbc,0x7f,0x11,0xd9,0x5c,0x41,0x1f,0x10,0x5a,0xd8,
	0x0a,0xc1,0x31,0x88,0xa5,0xcd,0x7b,0xbd,0x2d,0x74,0xd0,0x12,0xb8,0xe5,0xb4,0xb0,
	0x89,0x69,0x97,0x4a,0x0c,0x96,0x77,0x7e,0x65,0xb9,0xf1,0x09,0xc5,0x6e,0xc6,0x84,
	0x18,0xf0,0x7d,0xec,0x3a,0xdc,0x4d,0x20,0x79,0xee,0x5f,0x3e,0xd7,0xcb,0x39,0x48
};
b32 FK[4] = {
	0xa3b1bac6, 0x56aa3350, 0x677d9197, 0xb27022dc
};
b32 CK[32] = {
	0x00070e15, 0x1c232a31, 0x383f464d, 0x545b6269,
	0x70777e85, 0x8c939aa1, 0xa8afb6bd, 0xc4cbd2d9,
	0xe0e7eef5, 0xfc030a11, 0x181f262d, 0x343b4249,
	0x50575e65, 0x6c737a81, 0x888f969d, 0xa4abb2b9,
	0xc0c7ced5, 0xdce3eaf1, 0xf8ff060d, 0x141b2229,
	0x30373e45, 0x4c535a61, 0x686f767d, 0x848b9299,
	0xa0a7aeb5, 0xbcc3cad1, 0xd8dfe6ed, 0xf4fb0209,
	0x10171e25, 0x2c333a41, 0x484f565d, 0x646b7279
};

b32 S(b32 b); // 查S盒的函数B 
b32 Left(b32 a, short length); // 循环左移函数 
b32 L1(b32 a); // 线性变换L
b32 L2(b32 a); // 线性变换L'
b32 T(b32 a, short mode); // 合成变换T
void extendFirst(b32 MK[], b32 K[]); // 密钥扩展算法第一步
void extendSecond(b32 RK[], b32 K[]); // 密钥扩展算法第二步
void getRK(b32 MK[], b32 K[], b32 RK[]); // 轮密钥获取
void iterate32(b32 X[], b32 RK[]); // 迭代
void reverse(b32 X[], b32 Y[]); // 反转
void encrypt(b32 X[], b32 RK[], b32 Y[]);
void decrypt(b32 X[], b32 RK[], b32 Y[]);


b32 S(b32 b) {
	b8 a[4];
	a[0] = b / 0x1000000;
	a[1] = b / 0x10000;
	a[2] = b / 0x100;
	a[3] = b;
	b = Sbox[a[0]] * 0x1000000 + Sbox[a[1]] * 0x10000 + Sbox[a[2]] * 0x100 + Sbox[a[3]];
	return b;
}


b32 Left(b32 a, short length) {
	short i;
	for (i = 0; i < length; i++) {
		a = a * 2 + a / 0x80000000;
	}
	return a;
}


b32 L1(b32 a) {
	return ((a ^ Left(a, 2)) ^ Left(a, 10)) ^ (Left(a, 18) ^ Left(a, 24));
}


b32 L2(b32 a) {
	return a ^ Left(a, 13) ^ Left(a, 23);
}


b32 T(b32 a, short mode) {

	return mode == 1 ? L1(S(a)) : L2(S(a));
}


void extendFirst(b32 MK[], b32 K[]) {
	int i;
	for (i = 0; i < 4; i++) {
		K[i] = MK[i] ^ FK[i];
	}
}


void extendSecond(b32 RK[], b32 K[]) {
	short i;
	for (i = 0; i < 32; i++) {
		K[(i + 4) % 4] = K[i % 4] ^ T(K[(i + 1) % 4] ^ K[(i + 2) % 4] ^ K[(i + 3) % 4] ^ CK[i], 2);
		RK[i] = K[(i + 4) % 4];
	}
}

void getRK(b32 MK[], b32 K[], b32 RK[]) {
	extendFirst(MK, K);
	extendSecond(RK, K);
}



void iterate32(b32 X[], b32 RK[]) {
	short i;
	for (i = 0; i < 32; i++) {
		X[(i + 4) % 4] = X[i % 4] ^ T(X[(i + 1) % 4] ^ X[(i + 2) % 4] ^ X[(i + 3) % 4] ^ RK[i], 1);
	}
}


void reverse(b32 X[], b32 Y[]) {
	Y[0] = X[3];
	Y[1] = X[2];
	Y[2] = X[1];
	Y[3] = X[0];
}

void encrypt(b32 X[], b32 RK[], b32 Y[]) {
	iterate32(X, RK);
	reverse(X, Y);
}

void decrypt(b32 X[], b32 RK[], b32 Y[]) {
	b32 reverseRK[32];
	for (int i = 0; i < 32; i++) {
		reverseRK[i] = RK[32 - 1 - i];
	}
	iterate32(X, reverseRK);
	reverse(X, Y);
}

void Thread(b32** X, b32* Key, int i)
{
	b32 K[4]; // 密钥 
	b32 RK[32]; // 轮密钥
	b32 Y[4]; // 密文 
	for (int p = 0; p < 1; p++)
	{
		for (int j = 0; j < 3; j++)
		{
			getRK(K, Key, RK);
			encrypt(X[j * 4 + i], RK, Y);
			decrypt(Y, RK, X[j * 4 + i]);
		}
	}
}

void more_thread(b32** X, b32* Key)
{
	thread* threads = new thread[thread_num];
	int i = 0;
	for (i = 0; i < thread_num; i++)
		threads[i] = thread(Thread, ref(X), ref(Key),i);
	for (i = 0; i < thread_num; i++)
		threads[i].join();
}

void Thread12(b32** X, b32* Key, int i)
{
	b32 K[4]; // 密钥 
	b32 RK[32]; // 轮密钥
	b32 Y[4]; // 密文 
	for (int p = 0; p < 1; p++)
	{
		getRK(K, Key, RK);
		encrypt(X[i], RK, Y);
		decrypt(Y, RK, X[i]);
	}
}

void more_thread12(b32** X, b32* Key)
{
	thread* threads = new thread[3*thread_num];
	int i = 0;
	for (i = 0; i < 3*thread_num; i++)
		threads[i] = thread(Thread12, ref(X), ref(Key), i);
	for (i = 0; i < 3*thread_num; i++)
		threads[i].join();
}

void Thread24(b32** X, b32* Key, int i)
{
	i = i % 12;
	b32 K[4]; // 密钥 
	b32 RK[32]; // 轮密钥
	b32 Y[4]; // 密文 
	for (int p = 0; p < 5; p++)
	{
		getRK(K, Key, RK);
		encrypt(X[i], RK, Y);
		decrypt(Y, RK, X[i]);
	}
}

void more_thread24(b32** X, b32* Key)
{
	thread* threads = new thread[6 * thread_num];
	int i = 0;
	for (i = 0; i < 6 * thread_num; i++)
		threads[i] = thread(Thread24, ref(X), ref(Key), i);
	for (i = 0; i < 6 * thread_num; i++)
		threads[i].join();
}

int main(void) {
	b32* X[12]; // 明文 
	for (int i = 0; i < 12; i++)
	{
		X[i] = new b32[4];
	}
	b32 Key[4]; // 密钥 
	b32 RK[32]; // 轮密钥  
	b32 K[4];
	b32 Y[4]; // 密文 
	X[0][0] = 0x01234567; X[0][1] = 0x89abcdef; X[0][2] = 0xfedcba98; X[0][3] = 0x76543210;
	X[1][0] = 0x15ad15ef; X[1][1] = 0xaef4151b; X[1][2] = 0xe15c15da; X[1][3] = 0x384d84ea;
	X[2][0] = 0x84a1d8e2; X[2][1] = 0x89abcdef; X[2][2] = 0xfedcba98; X[2][3] = 0x76543210;
	X[3][0] = 0xeda15841; X[3][1] = 0xffffffff; X[3][2] = 0x55555555; X[3][3] = 0xaaaaaaaa;
	X[4][0] = 0xaaaaaaaa; X[4][1] = 0xeeeeeeee; X[4][2] = 0x77777777; X[4][3] = 0x51919191;
	X[5][0] = 0xbbbbbbbb; X[5][1] = 0xcccccccc; X[5][2] = 0x44444444; X[5][3] = 0x25195191;
	X[6][0] = 0xeeeeeeee; X[6][1] = 0x37542542; X[6][2] = 0x99999999; X[6][3] = 0x14814544;
	X[7][0] = 0xdddddddd; X[7][1] = 0xcaadedab; X[7][2] = 0x88888888; X[7][3] = 0x25181484;
	X[8][0] = 0xffffffff; X[8][1] = 0x57257689; X[8][2] = 0x22222222; X[8][3] = 0x25154841;
	X[9][0] = 0xcccccccc; X[9][1] = 0xbbbbbbbb; X[9][2] = 0x11111118; X[9][3] = 0xadefbcde;
	X[10][0] = 0x85855727; X[10][1] = 0xcccccccc; X[10][2] = 0x55555a98; X[10][3] = 0x77773210;
	X[11][0] = 0x75675584; X[11][1] = 0xdddddddd; X[11][2] = 0x7777ba98; X[11][3] = 0x76546666;
	Key[0] = 0x27542572; Key[1] = 0xaaaaaaaa; Key[2] = 0xfedcba98; Key[3] = 0x76543210;
	//printf("轮密钥\n");
	auto norm_start = std::chrono::steady_clock::now();
	for (int j = 0; j < 1; j++)
	{
		for (int i = 0; i < 12; i++)
		{
			getRK(Key, K, RK);
			encrypt(X[i], RK, Y);
			decrypt(Y, RK, X[i]);
		}
	}
	auto norm_end = std::chrono::steady_clock::now();
	std::chrono::duration<double> norm_time1 = norm_end - norm_start;
	printf("单线程执行时间为：%f\n", norm_time1.count()/12);
	printf("单线程吞吐量为：%f\n", 12/norm_time1.count());
	norm_start = std::chrono::steady_clock::now();
	more_thread(X, Key);
	norm_end = std::chrono::steady_clock::now();
	norm_time1 = norm_end - norm_start;
	printf("4线程执行时间为：%f\n", norm_time1.count()/12);
	printf("4线程吞吐量为：%f\n", 12/norm_time1.count());
	norm_start = std::chrono::steady_clock::now();
	more_thread12(X, Key);
	norm_end = std::chrono::steady_clock::now();
	norm_time1 = norm_end - norm_start;
	printf("12线程执行时间为：%f\n", norm_time1.count() / 12);
	printf("12线程吞吐量为：%f\n", 12 / norm_time1.count());
	/*norm_start = std::chrono::steady_clock::now();
	more_thread24(X, Key);
	norm_end = std::chrono::steady_clock::now();
	norm_time1 = norm_end - norm_start;
	printf("24线程执行时间为：%f\n", norm_time1.count() / 120);
	printf("24线程吞吐量为：%f\n", 120 / norm_time1.count());*/
	return 0;
}
