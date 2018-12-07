# EP2 - Redes de Computarodes
## Denial of Service attacks
Artigo: https://www.cs.ucf.edu/~dcm/Teaching/COT4810-Spring2011/Literature/DenialOfServiceAttacks.pdf

### Requisitos e Instalação do ns3
O trabalho foi desenvolvido no Ubuntu Desktop versão 14.4 LTS usando uma máquina virtual no VirtualBox
link para download: http://releases.ubuntu.com/14.04/

Depois de terminar a instalação do Ubuntu, abra o terminal e execute o seguinte comando para instalar os pré-requisitos:
```
sudo apt-get install gcc g++ python python-dev mercurial bzr gdb valgrind gsl-bin libgsl0-dev libgsl0ldbl flex bison tcpdump sqlite sqlite3 libsqlite3-dev libxml2 libxml2-dev libgtk2.0-0 libgtk2.0-dev uncrustify doxygen graphviz imagemagick texlive texlive-latex-extra texlive-generic-extra texlive-generic-recommended texinfo dia texlive texlive-latex-extra texlive-extra-utils texlive-generic-recommended texi2html python-pygraphviz python-kiwi python-pygoocanvas libgoocanvas-dev python-pygccxml git
```

O próximo passo é baixar e extrair o ns-3.19, execute os comandos:
```
mkdir ns3
cd ns3
wget http://www.nsnam.org/release/ns-allinone-3.19.tar.bz2
tar xjf ns-allinone-3.19.tar.bz2
cd ns-allinone-3.19/
```

Compile o ns-3.19:
`./build.py --enable-examples --enable-tests`

Na pasta ns-3.19, configure e compile usando o waf:
```
cd ns-3.19
./waf -d debug --enable-examples --enable-tests configure
```

Execute os testes para verificar se a instalação foi feita corretamente.
`./test.py`

### Executar
Clone esse repositório ou baixe o arquivo simulacao-DoS.cc e coloque ele na pasta ns-3.19/scratch. Depois compile e execute usando o waf:
```
./waf build
./waf --run simulacao-DoS --vis
```
