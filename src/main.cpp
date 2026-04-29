#include "common.h"

int main() {
	Graph G = buildPetersen();

	cout << "girth = " << girth(G).size << endl;
	cout << "cyclic cut = " << cyclicCut(G) << endl;

	Graph G1 = buildHexagonalPrism();

	cout << "girth = " << girth(G1).size << endl;
	cout << "cyclic cut = " << cyclicCut(G1) << endl;

	Graph G2 = buildMobiusLadder10();

	cout << "girth = " << girth(G2).size << endl;
	cout << "cyclic cut = " << cyclicCut(G2) << endl;

	Graph G3 = special4();

	cout << "girth = " << girth(G3).size << endl;
	cout << "cyclic cut = " << cyclicCut(G3) << endl;

	Graph G4 = largeOneGirth3();

	cout << "girth = " << girth(G4).size << endl;
	cout << "cyclic cut = " << cyclicCut(G4) << endl;

	return 0;
}
