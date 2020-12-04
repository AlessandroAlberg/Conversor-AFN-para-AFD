#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <queue>

#define qtdestados 100
#define qtdsimbolos 2
using namespace std;

vector <string> picotar ( string modelo ) {
	vector <string> retorno;
	string aux = "";

	for(int i=0;i<modelo.size();i++){
		if ( modelo[i] == ',' || modelo[i] == ' ' ){
			if ( aux != "" ) retorno.push_back(aux);
			aux = "";
		} else {
			aux += modelo[i];
		}

		if ( i == modelo.size() - 1 ){
			if ( aux != "" ) retorno.push_back(aux);
		}
	}
	return retorno;
}

class AFN {
	public:
		string estadoInicial;
		vector <string> estados;
		vector <string> terminais;
		vector <string> estadosfinais;
		map <string,int> :: iterator it;

		string matriz[qtdestados][qtdsimbolos];

		vector < string > AFD;
		queue < string > estadoparaconversao;

		void addEstados ( string conjuntoDeEstados ){
			estados = picotar(conjuntoDeEstados);
		}

		void addEstadosFinais ( string conjuntoDeEstadosFinais ){
			estadosfinais = picotar(conjuntoDeEstadosFinais);
		}

		void addSimbolos ( string conjuntoDeSimbolos ){
			terminais = picotar(conjuntoDeSimbolos);
		}

		void setEstadoInicial ( string estadoInicial ){
			this->estadoInicial = estadoInicial;
			estadoparaconversao.push(estadoInicial);
		}

		void addTransicao ( string inicio, string fim, string simbolo ){
			if ( matriz[getIndiceEstado(inicio)][getIndiceSimbolo(simbolo)].size() == 0 ){
				matriz[getIndiceEstado(inicio)][getIndiceSimbolo(simbolo)] = fim;
			} else {
				matriz[getIndiceEstado(inicio)][getIndiceSimbolo(simbolo)] += ", " + fim;
			}
		}

		int getIndiceEstado ( string estado ){
			for(int i=0;i<estados.size();i++){
				if ( estados[i] == estado ) return i;
			}
			return 0;
		}
		int getIndiceSimbolo ( string simbolo ){
			for(int i=0;i<terminais.size();i++){
				if ( terminais[i] == simbolo ) return i;
			}
			return 0;
		}

		string pegarPossibilidades ( string estado ){
			map <string, int> mapa;
			string possiveis;
			possibilidades(mapa,estado);

			for(it = mapa.begin();it != mapa.end();it++){
				if ( it != mapa.begin() ) possiveis += ", ";
				possiveis += it->first;
			}
			return possiveis;
		}

		void possibilidades ( map <string,int> & mapa, string estado ){
			vector <string> estadosAlcancaveis = picotar(estado);
			for(int i=0;i<estadosAlcancaveis.size();i++){
				it = mapa.find(estadosAlcancaveis[i]);
				if ( it == mapa.end()){
					mapa.insert( pair<string,int>(estadosAlcancaveis[i],1));
					possibilidades(mapa,matriz[getIndiceEstado(estadosAlcancaveis[i])][getIndiceSimbolo("*")]);
				}
			}
		}

		void imprimirMatriz(){
			for(int i=0;i<estados.size();i++){
				for(int j=0;j<terminais.size();j++){
					cout << estados[i] << "-" << terminais[j] << " = " << matriz[getIndiceEstado(estados[i])][getIndiceSimbolo(terminais[j])] << endl;
				}
				cout << endl;
			}
		}

		string distinct ( string todosestados ){
			map <string,int> mapa;
			map <string,int> :: iterator it;
			string novovalor;

			vector <string> valores = picotar(todosestados);
			for(int i=0;i<valores.size();i++){
				it = mapa.find(valores[i]);
				if ( it == mapa.end() ){
					mapa.insert( pair<string,int>(valores[i],1));
				}
			}
			for(it = mapa.begin();it != mapa.end();it++){
				if ( it != mapa.begin()) novovalor += ", ";
				novovalor += it->first;
			}

			return novovalor;
		}

		string adjacentes ( string estado, string simbolo ){
			string nova = matriz[getIndiceEstado(estado)][getIndiceSimbolo(simbolo)];
			nova += "," + pegarPossibilidades(nova);
			return nova;
		}

		string fatiar ( string estado, string simbolo ){
			vector <string> novo = picotar(estado);
			string retorno;
			for(int i=0;i<novo.size();i++){
				retorno += adjacentes(novo[i],simbolo) + ", ";
			}
			retorno = distinct(retorno);
			return retorno;
		}

		string verificaEstado ( string estado ){
			vector <string> picotado = picotar(estado);
			string retorno = "";

			if ( picotado.size() == 1 && picotado[0] == estadoInicial ) retorno += "->";

			bool final = false;
			for(int i=0;i<picotado.size();i++){
				for(int j=0;j<estadosfinais.size();j++){
					if ( picotado[i] == estadosfinais[j]) final = true;
				}
			}
			if ( final == true ) return (retorno + "*");
			return retorno;
		}

		void converter () {
			map < string, int > mapa;
			map < string, int> :: iterator it;

			while ( !estadoparaconversao.empty() ){
				string estado = estadoparaconversao.front();
				estadoparaconversao.pop();
				for(int i=0;i<terminais.size();i++){
					if ( terminais[i] != "*" ){
						string final = fatiar(estado,terminais[i]);
						string query = verificaEstado(estado) + "{ " + estado + " } (" + terminais[i] + ") = {" + final + "}";
						it = mapa.find(query);
						if ( it == mapa.end() && estado != "" ){
							estadoparaconversao.push(final);
							mapa.insert( pair<string,int> (query,1));
						}
					}
				}
			}

			for(it = mapa.begin(); it!= mapa.end();it++){
				cout << it->first << endl;
			}

		}
};


int main()
{
    AFN automato;
    string base, result, temp, initial, resultadoTransicao;
    string matrizAFD [100][3];
    base = "q";

    FILE *file;
    file = fopen("testadao.jff", "r");
    char ch;
    char vet[110];//vetor q faz as buscas
    int i=0; //serve pra organizar o vetor vet de todos caractere
    int y=-1; // contador de estados
    int fini=0,ini=0;//quantidade de estado inicial e final
    int in=0;//id do estado inicial
    int fi[100];// id dos estados finais

    if (file == NULL)
    {
        printf("Arquivo nao pode ser aberto\n");
        return 0;
    }

    while( (ch=fgetc(file))!= EOF ){
       cout<<ch;

       if(ch!=';'){//coloca cada caracter no vetor, até chegar no ";"
        vet[i]=ch;
        if(vet[i]=='"'&&vet[i-1]=='='&&vet[i-2]=='d'&&vet[i-3]=='i'){
        y++;
       }
        if(vet[i]=='t'&&vet[i-1]=='i'&&vet[i-2]=='n'&&vet[i-3]=='i'){
             in=y;ini++;

        }
        if(vet[i]=='a'&&vet[i-1]=='n'&&vet[i-2]=='i'&&vet[i-3]=='f'){

           fi[fini]=y; fini++;
        }
        i++;
       }
       if(ch==';'){//começa a contar da primeira posição do vetor novamente
        i=0;}
    }
    fclose(file);
    i=0;
//////////////////////////////////////////////////////////////////////
    FILE *file0;
        file0 = fopen("testadao.jff", "r");
        char vft[110];//from
        int f=0;//contador de from, to, read.
        char vtt[110];//to
        char vrt[110];//read
        string matriz [100][3];
        int linha=0; // parametros matriz
        int z=0;//contador dos vetores
        if (file0 == NULL)
        {
            printf("Arquivo nao pode ser aberto\n");
            return 0;
        }
        while( (ch=fgetc(file0))!= EOF ){

           if(ch!=';'){//coloca cada caracter no vetor, até chegar no ";"
            vft[z]=ch;
            if(vft[z]=='r'&&vft[z-1]=='f'&&vft[z-2]=='/'&&vft[z-3]=='<'){
            matriz [linha][0]=vft[z-4]; linha++;f++;}
            z++;}
           if(ch==';'){//começa a contar da primeira posição do vetor novamente
            z=0;}}
        fclose(file0);
        z=0;linha=0;
//////////////////////////////////////////////////////////////////////
    FILE *file1;
    file1 = fopen("testadao.jff", "r");
    if (file1 == NULL)
        {
            printf("Arquivo nao pode ser aberto\n");
            return 0;
        }
    while( (ch=fgetc(file1))!= EOF ){

           if(ch!=';'){//coloca cada caracter no vetor, até chegar no ";"
            vtt[z]=ch;
            if(vtt[z]=='o'&&vtt[z-1]=='t'&&vtt[z-2]=='/'&&vtt[z-3]=='<'){
            matriz [linha][1]=vtt[z-4]; linha++;}
            z++;}
          if(ch==';'){//começa a contar da primeira posição do vetor novamente
            z=0;}}
        fclose(file1);
        z=0;linha=0;
//////////////////////////////////////////////////////////////////////
    FILE *file2;
    file2 = fopen("testadao.jff", "r");
    if (file2 == NULL)
        {
            printf("Arquivo nao pode ser aberto\n");
            return 0;
        }
    while( (ch=fgetc(file2))!= EOF ){

           if(ch!=';'){//coloca cada caracter no vetor, até chegar no ";"
            vrt[z]=ch;
            if(vrt[z]=='e'&&vrt[z-1]=='r'&&vrt[z-2]=='/'&&vrt[z-3]=='<'){
            matriz [linha][2]=vrt[z-4]; linha++;}
            z++;}
          if(ch==';'){//começa a contar da primeira posição do vetor novamente
            z=0;}}
        fclose(file2);
        z=0;linha=0;
////////////////////////////////////////////////////////////////////////
    for(int i = 0; i <= y; i++){
            if(i == y){
                temp = base + to_string(i);
                result = result + temp;
            }
            else{
                temp = base + to_string(i) + ",";
                result = result + temp;
            }
    }

    automato.addEstados(result);
    initial = base + to_string(ini-1);
    automato.setEstadoInicial(initial);

    string estadoFinal;
    cout<<endl;
    cout<<"quantidade de estados comecando de zero: "<<y<<endl<<endl;
    cout<<"quantidade de estado inicial: "<<ini<<endl<<endl;
    cout<<"quantidade de estado final: "<<fini<<endl<<endl;
    cout<<"estado inicial: "<<in<<endl<<endl;
    for(int g=0;g<fini;g++){
        cout<<"estado final: "<<fi[g]<<endl<<endl;
        estadoFinal = base + to_string(fi[g]);
        automato.addEstadosFinais(estadoFinal);
    }
    automato.addSimbolos("0,1");

    cout<<endl;

    for(int b=0;b<f;b++){
    cout<<"saindo do estado "<<matriz[b][0]<<" indo para o estado "<<matriz[b][1]<<" se receber "<<matriz[b][2]<<endl;
    }

    //Adicionando transicao e convertendo

    cout<<"\n\Conversao:\n";

    for(int b=0;b<f;b++){
        automato.addTransicao(base + matriz[b][0], base + matriz[b][1], matriz[b][2]);
    }

    automato.converter();

    ofstream f_out;
    f_out.open("CONVERTIDO.txt");

    if(! f_out.good())
        return -1;

    else cout << "Arquivo criado!";

    string str = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><!--Created with JFLAP 6.4.--><structure>&#13;<type>fa</type>&#13;<automaton>&#13;<!--The list of states.-->&#13;";
    f_out << str;

    f_out.close();

   return 0;
}

