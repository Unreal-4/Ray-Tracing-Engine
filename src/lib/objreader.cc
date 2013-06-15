// File: objreader.cc
// Date: Sat Jun 15 22:28:46 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#include <cstring>
#include "lib/objreader.hh"
using namespace std;

const int LINE_MAX_LEN = 1000;
// assume that all vertex appears before face
void ObjReader::read_in(string fname, Mesh* mesh) {
	ifstream fin(fname);
	static char input[LINE_MAX_LEN];
	int nnorm = 0;
	bool startf = false;
	while (fin.getline(input, LINE_MAX_LEN, '\n')) {
		if (input[0] == '#' || strlen(input) <= 1)
			continue;
		switch (input[0]) {
			case 'v':
			{
				if (input[1] == ' ') {	// v x y z
					real_t x, y, z;
					sscanf(input + 2, "%lf %lf %lf", &x, &y, &z);
					mesh->add_vertex(Vec(x, y, z));
				} else if (input[1] == 'n') {  // vn x y z
					real_t x, y, z;
					sscanf(input + 3, "%lf %lf %lf", &x, &y, &z);
					mesh->set_norm(nnorm++, Vec(x, y, z));
				} else {
					cout << input << endl;
					error_exit("unrecognized format");
				}
				break;
			}
			case 'f':
			{				// XXX some model is 'f 1 2 3 4 5 6'
				if (!startf) {
					startf = true;
					mesh->transform_vtxs();
				}
				int x, y, z;
				sscanf(input + 2, "%d %d %d", &x, &y, &z);
				if (x != y && y != z && x != z)
					mesh->add_face(x - 1, y - 1, z - 1);
				break;
			}
			case 'g':
				continue;
				break;
			default:
				cout << input << endl;
				break;
		}
	}
	fin.close();
	if (nnorm == mesh->vtxs.size()) {
		cout << "use vn" << endl;
		mesh->smooth = true;
	}
	cout << "nvtx: " << mesh->vtxs.size() << ", nface: " << mesh->faces.size() << endl;
}



