#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
   
   if (argc == 3){
   	
		FILE *melIn, *melOut;
		int noVertices, noFaces, noBones, version;
		int res[3], vertInd[3];
		char tag[8], type[12], bonename[16];
		unsigned char color[12];
		double coord[3], nv[3], uv[6], head[3], tail[3], transform[15];
		float coord_c[3], nv_c[3], uv_c[6], head_c[3], tail_c[3], transform_c[15];
		int noAffBones, index, smooth;
		double weight, roll, length;
		float weight_c, roll_c, length_c;
		
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
		fread (&noVertices, sizeof(int), 1, melIn);
	   fwrite (&noVertices, sizeof(int), 1, melOut);
		fread (&noFaces, sizeof(int), 1, melIn);
	   fwrite (&noFaces, sizeof(int), 1, melOut);
		fread (&noBones, sizeof(int), 1, melIn);
	   fwrite (&noBones, sizeof(int), 1, melOut);
		fread (res, sizeof(int), 3, melIn);
	   fwrite (res, sizeof(int), 3, melOut);
	   
		// vertexlist
	   for (int v = 0; v < noVertices; v++) {
			fread (coord, sizeof(double), 3, melIn);
			coord_c[0] = (float) coord[0];
			coord_c[1] = (float) coord[1];
			coord_c[2] = (float) coord[2];
	   	fwrite (coord_c, sizeof(float), 3, melOut);
			fread (nv, sizeof(double), 3, melIn);
			nv_c[0] = (float) nv[0];
			nv_c[1] = (float) nv[1];
			nv_c[2] = (float) nv[2];
	   	fwrite (nv_c, sizeof(float), 3, melOut);
			fread (&noAffBones, sizeof(int), 1, melIn);
	   	fwrite (&noAffBones, sizeof(int), 1, melOut);
	   	
			for (int b = 0; b < noAffBones; b++) {
				fread (&index, sizeof(int), 1, melIn);
	   		fwrite (&index, sizeof(int), 1, melOut);
				fread (&weight, sizeof(double), 1, melIn);
				weight_c = (float) weight;
	   		fwrite (&weight_c, sizeof(float), 1, melOut);
			}
		}
		
		// facelist
		for (int f = 0; f < noFaces; f++) {
			fread (vertInd, sizeof(int), 3, melIn);
	   	fwrite (vertInd, sizeof(int), 3, melOut);
			fread (&smooth, sizeof(int), 1, melIn);
	   	fwrite (&smooth, sizeof(int), 1, melOut);
			fread (nv, sizeof(double), 3, melIn);
			nv_c[0] = (float) nv[0];
			nv_c[1] = (float) nv[1];
			nv_c[2] = (float) nv[2];
	   	fwrite (nv_c, sizeof(float), 3, melOut);
			fread (color, sizeof(unsigned char), 12, melIn);
	   	fwrite (color, sizeof(unsigned char), 12, melOut);
			fread (uv, sizeof(double), 6, melIn);
			uv_c[0] = (float) uv[0];
			uv_c[1] = (float) uv[1];
			uv_c[2] = (float) uv[2];
			uv_c[3] = (float) uv[3];
			uv_c[4] = (float) uv[4];
			uv_c[5] = (float) uv[5];
	   	fwrite (uv_c, sizeof(float), 6, melOut);
		}
		
		// bones
		for (int b = 0; b < noBones; b++) {
			fread (&index, sizeof(int), 1, melIn);
	   	fwrite (&index, sizeof(int), 1, melOut);
			fread (bonename, sizeof(char), 16, melIn);
			if (strcmp(bonename, "ArmUp.R        ") ==0) {
				char bn[8] = "ArmUp.R\0";
				char empty[8];
	   		fwrite (bn, sizeof(char), 8, melOut);
	   		fwrite (empty, sizeof(char), 8, melOut);
			} else if (strcmp(bonename, "Head           ") == 0) {
				char bn[5] = "Head\0";
				char empty[11];
	   		fwrite (bn, sizeof(char), 5, melOut);
	   		fwrite (empty, sizeof(char), 11, melOut);
			} else if (strcmp(bonename, "Spine          ") == 0) {
				char bn[6] = "Spine\0";
				char empty[10];
	   		fwrite (bn, sizeof(char), 6, melOut);
	   		fwrite (empty, sizeof(char), 10, melOut);
			} else {
	   		fwrite (bonename, sizeof(char), 16, melOut);
	   	}
	   	
			if (version == 1) {
				fread (head, sizeof(double), 3, melIn);
				head_c[0] = (float) head[0];
				head_c[1] = (float) head[1];
				head_c[2] = (float) head[2];
		   	fwrite (head_c, sizeof(float), 3, melOut);
				fread (tail, sizeof(double), 3, melIn);
				tail_c[0] = (float) tail[0];
				tail_c[1] = (float) tail[1];
				tail_c[2] = (float) tail[2];
		   	fwrite (tail_c, sizeof(float), 3, melOut);
				fread (&roll, sizeof(double), 1, melIn);
				roll_c = (float) roll;
		   	fwrite (&roll_c, sizeof(float), 1, melOut);
			} else if (version == 2) {
				fread (transform, sizeof(double), 12, melIn);
				transform_c[0] = (float) transform[0];
				transform_c[1] = (float) transform[1];
				transform_c[2] = (float) transform[2];
				transform_c[3] = (float) transform[3];
				transform_c[4] = (float) transform[4];
				transform_c[5] = (float) transform[5];
				transform_c[6] = (float) transform[6];
				transform_c[7] = (float) transform[7];
				transform_c[8] = (float) transform[8];
				transform_c[9] = (float) transform[9];
				transform_c[10] = (float) transform[10];
				transform_c[11] = (float) transform[11];
				fwrite (transform_c, sizeof(float), 12, melOut);
				fread (&length, sizeof(double), 1, melIn);
				length_c = (float) length;
	   		fwrite (&length_c, sizeof(float), 1, melOut);
			}
		}
		
		fflush (melOut); fclose (melIn); fclose (melOut); 
		return 0;
	}
	return -1;
}


