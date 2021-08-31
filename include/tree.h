/********************************************************
 * tree- Tree b in disk                                 *
 *                                                      *
 * Author:  Lucas Batista dos Santos                    *
 *                                                      *
 ********************************************************/
#ifndef TREE_H
#define TREE_H

#include "node.h"
#include "typedFile.h"
#include "charSerial.h"
#include "record.h"

using namespace std;

template <class T, const unsigned int MIN_DEGREE>
class tree : protected typedFile<node<T, MIN_DEGREE>> {
    public:
        tree(const string name, const string type, const unsigned int version);
        virtual ~tree() {};
        void insert(T k);
        void remove(T k);
        int search(node<T, MIN_DEGREE> &n, T k);
        void print();
    protected:
        void insertNonFull(node<T, MIN_DEGREE> &x, unsigned long long int i, T k);
        void removeAux(node<T, MIN_DEGREE> &x, unsigned long long int i, T k);
        void removeLeaf(node<T, MIN_DEGREE> &x, unsigned long long int i, T k);
        T findSuccessor(node<T, MIN_DEGREE> &x, unsigned long long int i);
        T findPredecessor(node<T, MIN_DEGREE> &x, unsigned long long int i);
        void inOrder();
        void inOrderAux();
        void printAux(node<T, MIN_DEGREE> x);
        int searchAux(node<T, MIN_DEGREE>& x, T k);

        node<T, MIN_DEGREE> root;
};

template <class T, const unsigned int MIN_DEGREE>
tree<T, MIN_DEGREE>::tree(const string name, const string type, const unsigned int version) : typedFile<node<T, MIN_DEGREE>>(name, type, version){
    record<node<T, MIN_DEGREE>> record; //Mesmo sem chaves, a raiz ficará na memoria e no arquivo
    if (tree<T, MIN_DEGREE>::getRoot() == 0) {
        tree<T, MIN_DEGREE>::insertRecord(record);
    } else {
        tree<T, MIN_DEGREE>::readRecord(record, tree<T, MIN_DEGREE>::getRoot());
    }
    root = record.getData();
}

template <class T, const unsigned int MIN_DEGREE>
void tree<T, MIN_DEGREE>::insert(T k) {


    unsigned long long int i = tree<T, MIN_DEGREE>::getRoot(), posZ;

    if (root.sizeNode() == node<T, MIN_DEGREE>::MAX) {//Caso raiz cheia, split
        node<T, MIN_DEGREE> x, z;
        record<node<T, MIN_DEGREE>> rx, ry, rz;
        x.setChild(i, 0); //posição da antiga raiz
        x.setLeaf(false);

        //Aloca nova posição
        posZ = tree<T, MIN_DEGREE>::allocatePosition();
        x.splitChild(root, z, 0, posZ);

        rx.setData(x); ry.setData(root); rz.setData(z);//Atualiza o record para salvar

        tree<T, MIN_DEGREE>::writeRecord(rz, posZ);
        tree<T, MIN_DEGREE>::insertRecord(rx); //Atualiza a nova raiz
        tree<T, MIN_DEGREE>::writeRecord(ry, i);

        i = tree<T, MIN_DEGREE>::getRoot();//Atualiza index raiz
        insertNonFull(x, i, k);
        root = x;
    }
    else {
        insertNonFull(root, i, k);
    }
}

template <class T, const unsigned int MIN_DEGREE>
void tree<T, MIN_DEGREE>::remove(T k) {
    unsigned int i = tree<T, MIN_DEGREE>::getRoot();
    removeAux(root, i, k);
}

template <class T, const unsigned int MIN_DEGREE>
int tree<T, MIN_DEGREE>::search(node<T, MIN_DEGREE>& x, T k) {
   x = root;
   return searchAux(x, k);
}

template <class T, const unsigned int MIN_DEGREE>
void tree<T, MIN_DEGREE>::print() {
   // cout << "[";
    printAux(root);
    //cout << "]" << endl;
}

template <class T, const unsigned int MIN_DEGREE>
void tree<T, MIN_DEGREE>::insertNonFull(node<T, MIN_DEGREE>& x, unsigned long long int ix, T k) {
    int i = x.sizeNode() - 1;
    record<node<T, MIN_DEGREE>> rx;
    if(x.isLeaf()) { //Se folha insira
        x.insertKey(k);
        rx.setData(x);
        tree<T, MIN_DEGREE>::writeRecord(rx, ix);
    } else { // Defina a raiz para inserir a chave
        node<T, MIN_DEGREE> y;
        record<node<T, MIN_DEGREE>> ry;
        while (i >= 0 && k < x.getKey(i)) {
            i--;
        }
        i++;
        if(tree<T, MIN_DEGREE>::readRecord(ry, x.getChild(i))){ // lendo do disco para a memória o filho apropriado
            y = ry.getData();
            if(y.sizeNode() == node<T, MIN_DEGREE>::MAX) { //caso filho cheio garanta que tenha menos que MAX chaves
                record<node<T, MIN_DEGREE>> rz;
                node<T, MIN_DEGREE> z;
                unsigned long long int posZ = tree<T, MIN_DEGREE>::allocatePosition(); //Aloca novo nó
                x.splitChild(y, z, i, posZ);
                //Atualiza os record para salvar
                rx.setData(x); ry.setData(y); rz.setData(z);
                tree<T, MIN_DEGREE>::writeRecord(rx, ix);
                tree<T, MIN_DEGREE>::writeRecord(ry, x.getChild(i));
                tree<T, MIN_DEGREE>::writeRecord(rz, posZ);

                if(k > x.getKey(i)) {
                    i++;
                }
            }
        }
        tree<T, MIN_DEGREE>::readRecord(ry, x.getChild(i)); // lendo do disco para a memória o filho apropriado
        y = ry.getData();
        insertNonFull(y, x.getChild(i), k);//Recursivamente insira k no filho apropriado
    }
}

template <class T, const unsigned int MIN_DEGREE>
void tree<T, MIN_DEGREE>::removeAux(node<T, MIN_DEGREE> &x, unsigned long long int i, T k) {

    unsigned int pos = x.hasKey(k); //Se contém chave
    record<node<T, MIN_DEGREE>> rx;

    if(x.isLeaf()) { // Case 1
        if(pos != node<T, MIN_DEGREE>::NOT_FOUND) { //Verifica se chave está na página, remove k de x
            x.removeKey(pos);
            rx.setData(x);
            tree<T, MIN_DEGREE>::writeRecord(rx, i);
        }
    } else { // Case 2,
        record<node<T, MIN_DEGREE>> ry, rz;
        node<T, MIN_DEGREE> y, z;
        T aux;
        if(pos != node<T, MIN_DEGREE>::NOT_FOUND && !x.isLeaf()) { //verifica k na página interna

            tree<T, MIN_DEGREE>::readRecord(ry, x.getChild(pos));
            y = ry.getData();
            if(y.sizeNode() > node<T, MIN_DEGREE>::MIN) { // 2a
                aux = findPredecessor(y, x.getChild(pos)); //busca k' de k e ja remove
                x.setKey(aux, pos);
                rx.setData(x);
                tree<T, MIN_DEGREE>::writeRecord(rx, i);

            } else {
                tree<T, MIN_DEGREE>::readRecord(rz, x.getChild(pos + 1));
                z = rz.getData();
                if(z.sizeNode() > node<T, MIN_DEGREE>::MIN) {//2b busca k' de k e remove
                    aux = findSuccessor(z, x.getChild(pos + 1));//busca e ja remove
                    x.setKey(aux, pos);
                    rx.setData(x);
                    tree<T, MIN_DEGREE>::writeRecord(rx, i);

                } else { //2c
                    unsigned long long int posDel = x.getChild(pos + 1);
                    x.mergeChildren(y, z, pos); // faz a fusao
                    rx.setData(x); ry.setData(y); rz.setData(z); //Atualizo os record apos merge(uniao)
                    tree<T, MIN_DEGREE>::writeRecord(rx, i);
                    tree<T, MIN_DEGREE>::writeRecord(ry, x.getChild(pos));
                    tree<T, MIN_DEGREE>::deleteRecord(rz, posDel);

                    if(!x.isLeaf() && x.sizeNode() == 0) { //verifica se a raiz nao ficou vazia
                        tree<T, MIN_DEGREE>::readRecord(rx, i); //raiz vazia
                        tree<T, MIN_DEGREE>::readRecord(ry, x.getChild(0)); //y cheio após uniao
                        tree<T, MIN_DEGREE>::deleteRecord(rx, x.getChild(0));//posição y
                        tree<T, MIN_DEGREE>::writeRecord(ry, i);//nova raiz
                        x = ry.getData();
                        removeAux(x, i, k);//remove k de x

                    } else {
                        tree<T, MIN_DEGREE>::readRecord(ry, x.getChild(pos));
                        y = ry.getData();
                        removeAux(y, x.getChild(pos), k);//apos uniao remove k
                    }
                }
            }
        } else {
            removeLeaf(x, i, k); // caso 3
        }
    }
}

template <class T, const unsigned int MIN_DEGREE>
void tree<T, MIN_DEGREE>::removeLeaf(node<T, MIN_DEGREE> &x, unsigned long long int i, T k) {
    bool flag = false;
    unsigned long long int pos;
    record<node<T, MIN_DEGREE>> rx, ry;
    node<T, MIN_DEGREE> y;
    pos = 0;
    while(pos <= x.sizeNode() && k > x.getKey(pos)) { //define a raiz que pode conter k
        pos++;
    }
    if(pos > x.sizeNode()) {
        pos--;
    }
    tree<T, MIN_DEGREE>::readRecord(ry, x.getChild(pos));
    y = ry.getData();
    if(y.sizeNode() == node<T, MIN_DEGREE>::MIN) {//verifica se y tem t - 1 chaves
        record<node<T, MIN_DEGREE>> rz;
        node<T, MIN_DEGREE> z;
        if(pos > 0) {
            tree<T, MIN_DEGREE>::readRecord(rz, x.getChild(pos - 1));
            z = rz.getData(); flag = true; //seta que o nó foi lido
        }
        if(flag && z.sizeNode() > node<T, MIN_DEGREE>::MIN) {
            x.rotateKeys(z, y, pos - 1);//Rotação direita
            rx.setData(x); ry.setData(y); rz.setData(z); //atualiza os record
            tree<T, MIN_DEGREE>::writeRecord(rx, i);
            tree<T, MIN_DEGREE>::writeRecord(ry, x.getChild(pos));
            tree<T, MIN_DEGREE>::writeRecord(rz, x.getChild(pos - 1));
        } else {
            flag = false;
            if(pos < x.sizeNode()) {
                tree<T, MIN_DEGREE>::readRecord(rz, x.getChild(pos + 1));
                z = rz.getData();
                flag = true;
            }
            if(flag && z.sizeNode() > node<T, MIN_DEGREE>::MIN) {
                x.rotateKeys(y, z, pos);//Rotação esquerda
                rx.setData(x); ry.setData(y); rz.setData(z);
                tree<T, MIN_DEGREE>::writeRecord(rx, i);
                tree<T, MIN_DEGREE>::writeRecord(ry, x.getChild(pos));
                tree<T, MIN_DEGREE>::writeRecord(rz, x.getChild(pos + 1));
            } else {
                if(pos == x.sizeNode()) {
                    unsigned int posDel = x.getChild(pos);//Caso a raiz perca ultima chave
                    tree<T, MIN_DEGREE>::readRecord(rz, x.getChild(pos - 1));
                    z = rz.getData();
                    x.mergeChildren(z, y, pos - 1); //uniao filho direita
                    rx.setData(x); ry.setData(y); rz.setData(z);
                    tree<T, MIN_DEGREE>::writeRecord(rx, i);
                    tree<T, MIN_DEGREE>::deleteRecord(ry, posDel);
                    tree<T, MIN_DEGREE>::writeRecord(rz, x.getChild(pos - 1));
                } else {
                    unsigned int posDel = x.getChild(pos + 1);
                    x.mergeChildren(y, z, pos); //uniao filho esquerda
                    rx.setData(x); ry.setData(y); rz.setData(z);//Atualizo os record apos merge
                    tree<T, MIN_DEGREE>::writeRecord(rx, i);
                    tree<T, MIN_DEGREE>::writeRecord(ry, x.getChild(pos));
                    tree<T, MIN_DEGREE>::deleteRecord(rz, posDel);
                }
            }
        }
    }
    flag = true;
    if(!x.isLeaf()  && x.sizeNode() == 0) { // verifica se raiz nao está vaza pos merge
        tree<T, MIN_DEGREE>::readRecord(rx, i);
        tree<T, MIN_DEGREE>::readRecord(ry, x.getChild(0));
        tree<T, MIN_DEGREE>::deleteRecord(rx, x.getChild(0));
        tree<T, MIN_DEGREE>::writeRecord(ry, i);
        x = ry.getData();
        flag = false;
    }
    pos = 0;
    while(pos < x.sizeNode() && k > x.getKey(pos)) {//posição que pode conter k
        pos++;
    }
    if(flag) {
        tree<T, MIN_DEGREE>::readRecord(ry, x.getChild(pos));
        y = ry.getData();
        removeAux(y, x.getChild(pos), k);
    } else {
        removeAux(x, i, k);
    }
}

template <class T, const unsigned int MIN_DEGREE>
T tree<T, MIN_DEGREE>::findSuccessor(node<T, MIN_DEGREE> &x, unsigned long long int i) {
    record<node<T, MIN_DEGREE>> rx;
    node<T, MIN_DEGREE> s;
    T k;
    if (x.isLeaf()) { // se for folha, remove k e retorne
        k = x.getKey(0);
        removeAux(x, i, k);
        return k;
    } else { //
        tree<T, MIN_DEGREE>::readRecord(rx, x.getChild(0));
        if(rx.getData().isLeaf()) { //verifica se filho de x é folha se for remova k e retorne
            T k = rx.getData().getKey(0);
            removeLeaf(x, i, k);
            return k;
        } else {
            s = rx.getData();
            return findSuccessor(s, x.getChild(x.getChild(0))); //busca o filho apropriado
        }
    }
}

template <class T, const unsigned int MIN_DEGREE>
T tree<T, MIN_DEGREE>::findPredecessor(node<T, MIN_DEGREE> &x, unsigned long long int i) {
    record<node<T, MIN_DEGREE>> rx;
    node<T, MIN_DEGREE> s;
    T k;
    if (x.isLeaf()) { // se for folha, remove k e retorne
        k = x.getKey(x.sizeNode() - 1);
        removeAux(x, i, k);
        return k;
    } else {
        tree<T, MIN_DEGREE>::readRecord(rx, x.getChild(x.sizeNode()));
        if(rx.getData().isLeaf()) {
            T k = rx.getData().getKey(x.sizeNode() - 1); //verifica se filho de x é folha se for remova k e retorne
            removeLeaf(x, i, k); //faz as mudanças e remove
            return k;
        } else {
            s = rx.getData();
            return findPredecessor(s, x.getChild(x.sizeNode()));
        }
    }
}

template <class T, const unsigned int MIN_DEGREE>
void tree<T, MIN_DEGREE>::printAux(node<T, MIN_DEGREE> x) {
    record<node<T, MIN_DEGREE>> rz;
    unsigned int i;

    for (i = 0; i < x.sizeNode(); i++) {
        if (!x.isLeaf()){
            tree<T, MIN_DEGREE>::readRecord(rz, x.getChild(i));
            printAux(rz.getData());
        }
        cout << " " << x.getKey(i).getCharacter();
    }
    if (!x.isLeaf()){
        tree<T, MIN_DEGREE>::readRecord(rz, x.getChild(i));
        printAux(rz.getData());
    }
}

template <class T, const unsigned int MIN_DEGREE>
int tree<T, MIN_DEGREE>::searchAux(node<T, MIN_DEGREE>& x, T k) {
   unsigned int i = 0;

    while (i < x.sizeNode() && k > x.getKey(i)) {
        i++;
    }
    if (i < x.sizeNode() && k == x.getKey(i)) {
        return i;
    } else {
        if(x.isLeaf()) {
            x.resize(0);
            return -1;
        } else {
            record<node<T, MIN_DEGREE>> rx;
            tree<T, MIN_DEGREE>::readRecord(rx, x.getChild(i));
            x = rx.getData();
            return searchAux(x, k);
        }
    }
}
#endif // TREE_H
