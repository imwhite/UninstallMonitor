/*
 * Utils.cc
 *
 *  Created on: 2016-3-22
 *      Author: white
 */

#include <stdio.h>
#include <string.h>

#ifndef UTILS_H_
#define UTILS_H_



bool writeFile(char *path, char *data) {
	FILE *file;
	if((file = fopen(path, "w")) == NULL){
		return false;
	}

	if((strlen(data)) > 0){
		fputs(data, file);
	}
	fclose(file);
	return true;

}


#endif /* UTILS_H_ */
