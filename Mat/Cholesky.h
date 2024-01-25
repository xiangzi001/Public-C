
template <typename T>
void LLT(MAT<T>& A, MAT<T>& L) {
	for (int j = 0; j < A.cols; j++)
		for (int i = j; i < A.rows; i++) {
			if (i == j) {
				T sum = 0;
				for (int k = 0; k < j; k++) {
					sum = sum + L[j][k] * L[j][k];
				}
				L[i][j] = sqrt(A[i][j] - sum);
			}
			else {
				T sum = 0;
				for (int k = 0; k < j; k++) {
					sum = sum + L[i][k] * L[j][k];
				}
				L[i][j] = (A[i][j] - sum) / L[j][j];
			}
		}
}