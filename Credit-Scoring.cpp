#include<stdio.h>
#include <ilcplex/ilocplex.h>
#include <iostream>
#include<fstream>
#include<string>
#include<stdlib.h>
#include <sstream>
ILOSTLBEGIN
int main() {

	int i = 0, k = 0, j = 0;
	//Notre vecteur des Y et notre matrice des X (les caracteristiques)
	int Y[200];
	double X[200][14];
	ifstream file("x.txt");
	ifstream f("y.txt");

	//Lecture du fichier y.txt et alimentation du vecteur Y
	while (f >> i) {
		Y[k++] = i;
	}
	f.close();

	while (!file.eof()) {
		string line;
		getline(file, line); // lit une ligne
		istringstream iss(line); // une seule ligne
		do {
			if (i == 200) //Fin de lecture, la derniere ligne donc je sors de ma boucle
				break;
			string mot;
			iss >> mot; // Division de chaque numero de la ligne
			if (mot.empty() || mot == " ") //Pour eviter de lire le vide de chaque ligne
				continue;
			X[i][j++] = atof(mot.c_str()); // convertir sub à  double et le stocker       
			if (j == 14) {
				j = 0;
				i++;
			}

		} while (iss); // Tant que la ligne n'est pas arrivée  à  sa fin
	}
	file.close();

	//********************************************************************************************
	//déclaration des trois classes principales 
	IloEnv env; // environnement : permet d utiliser les fonctions de Concert Technology 
	IloModel model(env); // modèle : représente le programme linéaire 
	IloCplex cplex(model); // classe Cplex : permet d 'accéder aux fonctions d'optimisation
	// ----------------------- VARIABLES ----------------------- 
	//déclaration des variables 
	
		IloNumVarArray a(env, 200, 0.0, IloInfinity, ILOFLOAT);
		IloNumVarArray w(env, 14, -IloInfinity, IloInfinity, ILOFLOAT);
		
		//IloNumVar c(env, -IloInfinity,IloInfinity, ILOFLOAT, "c");  model.add(c);  //le seuil c est considerée comme une variable

		double c = 0.5;
		model.add(a);
		model.add(w);
		// ----------------------- FONCTION OBJECTIF -----------------
		//déclaration de la fonction objectif

		IloObjective FctObj(env, IloSum(a), IloObjective::Minimize, "Fonction Objectif");
		model.add(FctObj);
		// ----------------------- CONTRAINTES ----------------------- 
		//déclaration des contraintes

		IloExpr xw;
		for (int k = 0; k < 200; k++) {
			
			model.add(a[k] >= 0);    //cette contrainte est faite pour imposer la positivité des ai 

			IloRange positivite(env, 0, a[k], IloInfinity,"positif");
			model.add(positivite);
			xw = X[k][0] * w[0];
			for (int j = 1; j < 14; j++)  //calcul de l'expression de somme(xij*wj)
			{
				xw += X[k][j] * (w[j]);
			}
			if (Y[k] == 1) {       //Bon client
				IloRange GoodContrainte(env, 0,a[k]-c+xw, IloInfinity, "Good Client");
				model.add(GoodContrainte);
			}
			else {                 //Mauvais client
				IloRange BadContrainte(env, 0,a[k]+c-xw, IloInfinity, "Bad Client");
				model.add(BadContrainte);
			}
		}
		// ------------------ AFFICHAGE ET OPTIMISATION ---------------- 
		// export du PL créé dans un fichier ModelScoring.lp 
		cplex.exportModel("ModelScoring.lp");
		// résolution 
		cplex.solve();
		// export de la solution dans un fichier texte nommé solutionScoring.txt
		cplex.writeSolution("solutionScoring.txt");
		// récupère la solution et l'affiche à l'écran 
		cout << "La valeur de la fonction objective est = " << cplex.getObjValue() << endl; 
		
		cout << "Les valeurs des déviations ai sont : "<< endl;  							
		for (int i = 0; i < 200; i++) {
			cout << "  a" << i+1 << " = " << cplex.getValue(a[i]) << endl;
		}

		cout << "Les valeurs des Poids Wj sont : " << endl;
		for (int j = 0; j < 13; j++) {
			cout << "  w" << j+1 << " = " << cplex.getValue(w[j]) << endl;
		}
		//cout << "la valeur du seuil est : "<<cplex.getValue(c)<< endl;
	env.end();
	system("pause");
	return 0;
}