
template <typename T>
class MAT {
public:
	int rows;
	int cols;
	T* data;

	MAT() :
		data(0), rows(-1), cols(-1)
	{
	}

	MAT(int r, int c) :
		data(new T[r * c]), rows(r), cols(c)
	{
	}

	template<class t>
	MAT(int r, int c, t* p) :
		rows(r), cols(c)
	{
		int s = r * c;
		data = new T[s];
		for (int i = 0; i < s; i++) {
			data[i] = p[i];
		}
	}

	~MAT() {
		delete[] data;
	}

	void init(int r, int c) {
		rows = r;
		cols = c;
		delete[] data;
		data = new T[r * c];
	}
	template<typename t>
	void init(int r, int c, t* p) {
		rows = r;
		cols = c;
		int s = r * c;
		delete[] data;
		data = new T[s];
		for (int i = 0; i < s; i++) {
			data[i] = p[i];
		}
	}
	void iden(int a) {
		rows = a;
		cols = a;
		int s = a * a;
		delete[] data;
		data = new T[s];
		for (int i = 0; i < s; i++)
			data[i] = 0;
		for (int i = 0; i < a; i++)
			data[i * (a + 1)] = 1;
	}
	void fill(int a) {
		int s = rows * cols;
		for (int i = 0; i < s; i++)
			data[i] = a;
	}
	
	T* operator [] (int r) {
		return (data + cols * r);
	}

	MAT operator * (MAT b) {
		T v;
		MAT n(rows, b.cols);
		for (int i = 0; i < n.rows; ++i) {
			for (int k = 0; k < cols; ++k) {
				v = data[i * cols + k];
				for (int j = 0; j < n.cols; ++j)
					n[i][j] += v * b[k][j];
			}
		}
		b.data = 0;
		return n;
	}

	MAT operator + (MAT b) {
		MAT n(rows, cols);
		for (int x = 0; x < n.rows; x++)
			for (int y = 0; y < n.cols; y++) {
				n.data[x * n.cols + y] = data[x * n.cols + y] + b.data[x * n.cols + y];
			}
		b.data = 0;
		return n;
	}

	MAT operator - (MAT b) {
		MAT n(rows, cols);
		for (int x = 0; x < n.rows; x++)
			for (int y = 0; y < n.cols; y++) {
				n.data[x * n.cols + y] = data[x * n.cols + y] - b.data[x * n.cols + y];
			}
		b.data = 0;
		return n;
	}

	//copy B to A
	void operator = (MAT B) {
		delete[] data;
		rows = B.rows;
		cols = B.cols;
		int s = rows * cols;
		data = new T[s];
		for (int i = 0; i < s; i++) {
			data[i] = B.data[i];
		}
		B.data = 0;
	}

	//exchange A and B
	void operator & (MAT& B) {
		int temp = rows;
		rows = B.rows;
		B.rows = temp;

		temp = cols;
		cols = B.cols;
		B.cols = temp;

		T* _temp = data;
		data = B.data;
		B.data = _temp;
	}
	//put A to B
	void operator >> (MAT& B) {
		B.rows = rows;
		rows = -1;
		B.cols = cols;
		cols = -1;
		delete[] B.data;
		B.data = data;
		data = 0;
	}
	//put B to A
	void operator << (MAT& B) {
		rows = B.rows;
		B.rows = -1;
		cols = B.cols;
		B.cols = -1;
		delete[] data;
		data = B.data;
		B.data = 0;
	}
};

template <typename T>
class MATtag {
public:
	int rows;
	int cols;
	T* data;

	T* operator [] (int r) {
		return (data + cols * r);
	}

	MATtag() :
		data(0), rows(-1), cols(-1)
	{
	}

	MATtag(MAT<T> B) :
		data(B.data), rows(B.rows), cols(B.cols)
	{
		B.data = 0;
	}
};

template<class T>
void print(MAT<T> A) {
	for (int x = 0; x < A.rows; x++) {
		for (int y = 0; y < A.cols; y++) {
			cout << A[x][y] << ",	";
		}
		cout << endl;
	}
	cout << endl;
	A.data = 0;
}

#include "Base.h"
#include "Norm.h"
#include "LU.h"
#include "QR.h"
#include "Cholesky.h"
#include "Eigen.h"
#include "SVD.h"
#include "Test.h"
