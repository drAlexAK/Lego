void SortShell(int* a, int size);
int GetMedian(int* a, int size);

//////////

/**
 * Returns median by array.
 * Unfortuantly, array size is limited by 32 * int by stupid robotc allocator
 * @param *a pointer to array
 * @param numbers of elements
 * @return median
 */
int GetMedian(int* a, int size)
{
	int middle = size / 2;
	int b[32];
	memcpy(b, a, sizeof(int) * size);
  SortShell(b, size);
  return b[middle];
}
/**
 * Sort int array ascending.
 * @param *a pointer to array
 * @param numbers of elements
 */
void SortShell(int* a, int size)
{
	for	(int m = size/2 ; m > 0; m /= 2)
	{
		for (int j = m; j < size; j++)
		{
			for (int i = j - m; i >= 0; i -= m)
			{
				if (a[i + m] >= a[i])
					break	;
				else
				{
					int t = a[i];
					a[i] = a[i + m];
					a[i + m] = t;
				}
			}
		}
	}
}
