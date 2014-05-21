#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
   
   if (argc == 3){
	   
		FILE *melIn, *melOut;
		int noPoses, noBones, version;
		int res[2];
		char tag[8], type[12];
		int keyTime;
		double quat[4], trans[3], scale[3];
		float quat_c[4], trans_c[3], scale_c[3];
			
		melIn = fopen (argv[1], "rb");
		if(!melIn) return -1;
		
		melOut = fopen(argv[2], "wb");
		if(!melOut) {
			fclose (melIn);
			return -1;
		}
		 
		// header
		fread (tag, sizeof(char), 8, melIn);
	   fwrite (tag, sizeof(char), 8, melOut);
		fread (type, sizeof(char), 12, melIn);
	   fwrite (type, sizeof(char), 12, melOut);
		fread (&version, sizeof(int), 1, melIn);
	   fwrite (&version, sizeof(int), 1, melOut);
		fread (&noBones, sizeof(int), 1, melIn);
	   fwrite (&noBones, sizeof(int), 1, melOut);
		fread (&noPoses, sizeof(int), 1, melIn);
	   fwrite (&noPoses, sizeof(int), 1, melOut);
		fread (res, sizeof(int), 2, melIn);
	   fwrite (res, sizeof(int), 2, melOut);
	   
		// poselist
	   for (int v = 0; v < noPoses; v++) {
			fread (&keyTime, sizeof(int), 1, melIn);
	   	fwrite (&keyTime, sizeof(int), 1, melOut);
			printf ("key[%d]\n", keyTime);
			// transformation bonelist
			for (int b = 0; b < noBones; b++) {
				fread (quat, sizeof(double), 4, melIn);
				quat_c[0] = (float) quat[0];
				quat_c[1] = (float) quat[1];
				quat_c[2] = (float) quat[2];
				quat_c[3] = (float) quat[3];
		   	fwrite (quat_c, sizeof(float), 4, melOut);
				fread (trans, sizeof(double), 3, melIn);
				trans_c[0] = (float) trans[0];
				trans_c[1] = (float) trans[1];
				trans_c[2] = (float) trans[2];
		   	fwrite (trans_c, sizeof(float), 3, melOut);
				fread (scale, sizeof(double), 3, melIn);
				scale_c[0] = (float) scale[0];
				scale_c[1] = (float) scale[1];
				scale_c[2] = (float) scale[2];
		   	fwrite (scale_c, sizeof(float), 3, melOut);
			}
		}
		
		fflush (melOut); fclose (melIn); fclose (melOut); 
		getchar();
		return 0;
	}
	getchar();
	return -1;
}


