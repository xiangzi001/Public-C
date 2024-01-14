
template <typename T>
void random(MAT<T>& A, int r, int c, int Big, int Small) {
	A.rows = r;
	A.cols = c;
	int s = r * c;
	delete[] A.data;
	A.data = new T[s];
	for (int i = 0; i < s; i++) {
		srand((int)(time(0)) + i);
		A.data[i] = (rand() % (Big - Small)) + Small;
	}
}

template<typename t>
MAT<t> T(MAT<t> A) {
	MAT<t> n(A.cols, A.rows);
	for (int i = 0; i < A.cols; i++)
		for (int j = 0; j < A.rows; j++) {
			n[i][j] = A[j][i];
		}
	A.data = 0;
	return n;
}
template<typename t>
MAT<t> trans(MAT<t> A) {
	MAT<t> n(A.cols, A.rows);
	for (int i = 0; i < A.cols; i++)
		for (int j = 0; j < A.rows; j++) {
			n[i][j] = A[j][i];
		}
	A.data = 0;
	return n;
}

template<typename t>
MAT<t> copy(MAT<t>& A) {
	MAT<t> n(A.cols, A.rows);
	for (int i = 0; i < A.cols * A.rows; i++)
		n.data[i] = A.data[i];
	return n;
}

template<typename T>
void times(MAT<T>& A, MAT<T>& B, MAT<T>& C) {
	int pos = C.rows * C.cols;
	T v;
	for (int i = 0; i < pos; i++) {
		C.data[i] = 0;
	}
	for (int i = 0; i < A.rows; ++i) {
		pos = i * A.cols;
		for (int k = 0; k < A.cols; ++k) {
			v = A.data[pos + k];
			for (int j = 0; j < B.cols; ++j)
				C.data[pos + j] += v * B.data[k * A.rows + j];
		}
	}
}

template<typename T>
void times_A_BT(MAT<T>& A, MAT<T>& B, MAT<T>& C) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < B.rows; j++) {
			C[i][j] = 0;
			for (int k = 0; k < A.cols; k++)
				C[i][j] = C[i][j] + A[i][k] * B[j][k];
		}
	}
}

template<typename T>
void times_AT_B(MAT<T>& A, MAT<T>& B, MAT<T>& C) {
	int size = A.cols * B.cols;
	for (int i = 0; i < size; i++)
		C.data[i] = 0;
	for (int k = 0; k < A.rows; k++) {
		for (int i = 0; i < A.cols; i++) {
			for (int j = 0; j < B.cols; j++) {
				C[i][j] = C[i][j] + A[k][i] * B[k][j];
			}
		}
	}
}

template<typename T>
T Det(MAT<T> n) {
	double r = 1;
	for (int y = 0; y < n.cols; y++) {
		if (n[y][y] == 0)
			for (int k = y + 1; k < n.rows; k++)
				if (n[k][y] != 0) {
					T temp;
					for (int i = 0; i < n.cols; i++) {
						temp = n[k][i];
						n[k][i] = n[y][i];
						n[y][i] = temp;
					}
					r = -r;
					break;
				}
		for (int x = 0; x < n.rows; x++) {
			if (x > y) {
				double temp(-n[x][y] / n[y][y]);
				for (int k = y; k < n.rows; k++)
					n[x][k] = n[x][k] + n[y][k] * temp;
			}
		}
	}
	for (int x = 0; x < n.rows; x++)
		r = r * n[x][x];
	n.data = 0;
	return r;
}

template<typename T>
T Det_c(MAT<T> n) {
	MAT<T> m(n.rows, n.cols, n.data);
	n.data = 0;
	return Det(m);
}
template<typename T>
int Rank(MAT<T>& A, T eps=0){
	int r, c;
	for (r = 0, c = 0; c < A.cols; c++) {

		// 1. 找到最大值
		int t = r;
		for (int i = r; i < A.cols; i++) {
			if (fabs(A[i][c]) > fabs(A[t][c]))
				t = i;
		}

		if (fabs(A[t][c]) < eps) continue;

		// 2. 交换到首行
		for (int i = c; i < A.rows; i++) swap(A[r][i], A[t][i]);

		// 3. 本行首列置为1
		for (int i = A.rows - 1; i >= c; i--) A[r][i] /= A[r][c];

		// 4. 下面的行首列置为0
		for (int i = r + 1; i < A.cols; i++) {
			if (fabs(A[i][c]) < eps) continue;
			for (int j = A.rows - 1; j >= c; j--) {
				A[i][j] -= A[i][c] * A[r][j]; // - 首航本列 * i, r首列商
			}
		}

		r++;
	}
	return r;
}

template<typename T>
int Rank_c(MAT<T>& A, T eps = 0) {
	MAT<T> B(copy(A));
	return Rank(B);
}

template<typename T>
void Solve(MAT<T>& A, MAT<T>& b) {
	if (A.rows == A.cols && b.rows == A.cols) {
		for (int y = 0; y < A.cols; y++) {
			if (A[y][y] == 0)
				for (int k = y + 1; k < A.rows; k++)
					if (A[k][y] != 0) {
						T temp;
						for (int i = 0; i < A.cols; i++) {
							temp = A[k][i];
							A[k][i] = A[y][i];
							A[y][i] = temp;
						}
						for (int i = 0; i < b.cols; i++) {
							temp = b[k][i];
							b[k][i] = b[y][i];
							b[y][i] = temp;
						}
						break;
					}
			for (int x = y + 1; x < A.rows; x++) {
				T temp(-A[x][y] / A[y][y]);
				for (int k = y; k < A.rows; k++)
					A[x][k] = A[x][k] + A[y][k] * temp;
				for (int k = 0; k < b.cols; k++)
					b[x][k] = b[x][k] + b[y][k] * temp;
			}
		}
		for (int i = 0; i < A.rows; i++)
			if (A[i][i] != 0) {
				T temp = 1 / A[i][i];
				for (int n = i; n < A.cols; n++)
					A[i][n] = A[i][n] * temp;
				for (int n = 0; n < b.cols; n++)
					b[i][n] = b[i][n] * temp;
			}
			else return;

		for (int y = A.cols - 1; y > 0; y--)
			for (int x = 0; x < y; x++) {
				for (int n = 0; n < b.cols; n++)
					b[x][n] = b[x][n] - A[x][y] * b[y][n];
			}
		return;
	}
}

template<typename T>
MAT<T> solve_c(MAT<T>& A, MAT<T>& b) {
	MAT<T> B(A.rows, A.cols, A.data);
	MAT<T> c(b.rows, b.cols, b.data);
	Solve(B, c);
	return c;
}

template<typename T>
MAT<T> Inv(MAT<T>& A) {
	MAT<T> I;
	I.iden(A.rows);
	Solve(A, I);
	return I;
}

template<typename T>
MAT<T> Inv_c(MAT<T>& A) {
	MAT<T> I;
	I.iden(A.rows);
	MAT<T> B(A.rows, A.cols, A.data);
	Solve(B, I);
	return I;
}
