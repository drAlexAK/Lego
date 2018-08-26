
typedef struct power{
	byte left;
	byte right;
}power;

task main()
{

	 power p;
	 p.left = 70;
	 p.right = 80;

	 power a[120];

	 for(int i = 0; i < 120; i ++){
	 a[i].left = i;
	 a[i].right = i;
	 }
	int lf = i >> 4;
	int rf = i - (lf << 4);


}
