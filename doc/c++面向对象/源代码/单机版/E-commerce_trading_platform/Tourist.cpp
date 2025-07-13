#include"Tourist.h"
#include"Discaonut.h"

int Tourist::getUserType(string userName) {
	return sheet.getUserType(userName);
}

void Tourist::updateSheet() {
	p.getPsheet();
	sheet.readInInformation();
}