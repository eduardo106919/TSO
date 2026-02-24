
# Guide 02 - Observability

This guide will allow you to try out different tools that allow observing and understanding the behavior of
applications without inspecting their source code.

**Tools/technologies**: We will be using the setup from Guide 0, and the code and binary provided along with this guide (the code is based on the one developed on **Guide 1**). You will be using tools such as `strace`, `perf`, `journalctl`, and `htop`.

**Learning Outcomes**: Explore monitoring, logging, tracing, and profiling tools. Understand the behavior of applications without source code analysis. Explore OS-level observability.


## Warmup

Após executar `time ./metanode`, o output do programa `metanode` é:
```
blocking
resuming
key 0.txt, size 0, refs 10000000
key 1.txt, size 1, refs 0
key 2.txt, size 2, refs 0
key 3.txt, size 3, refs 0
key 4.txt, size 4, refs 0
key 5.txt, size 5, refs 0
key 6.txt, size 6, refs 0
key 7.txt, size 7, refs 0
key 8.txt, size 8, refs 0
key 9.txt, size 9, refs 0
```

Podemos verificar a existência de 10 novos ficheiros, com nomes do tipo `0.txt`, `1.txt`, ... Cada um dos ficheiros tem 10MB de tamanho.

O output da contagem de tempo é:
```
real	0m2.130s
user	0m1.591s
sys	    0m1.343s
```


## Exercises


### Comparing versions 0 and 1

Versão 1 não escreve o conteúdo, os ficheiros tem 0 de tamanho.

Os ficheiros `1.txt`... são abertos para leitura, e ao efetuar escritas dá erro.

Output da execução `time ./metanode 1`:
```
real	0m2.474s
user	0m2.399s
sys  	0m0.765s
```
Após executar a versão 1 do programa, verificamos que os 10 ficheiros são criados, mas estão vazios (o seu tamanho é 0). Verificamos também que houve uma redução no tempo de execução por parte do sistema operativo.

Através do comando `strace` podemos descobrir a causa do problema:
```
strace -yy ./metanode 1
```
Após uma análise ao output, vemos que os ficheiros estão a ser abertos, mas a escrita não está a ser efetuada (devolvendo `-1`).
```
openat(AT_FDCWD<...>, "3.txt", O_RDONLY|O_CREAT|O_TRUNC, 0600) = 3<.../3.txt>
write(3<.../3.txt>, "nyEZFTTPAPGfiKQ3Y8eswj7hB42sjJWw"..., 10485760) = -1 EBADF (Bad file descriptor)
close(3<.../3.txt>) = 0
```

O problema está na system call `openat()`, mais precisamente nas flags selecionadas. Foram passadas as flags de **leitura**, criação e truncagem. Se um ficheiro está aberto unicamente para leitura, é evidente que as escritas vão falhar.


### Version 2

Output da versão 2 (`time ./metanode 2`):
```
real	0m37.712s
user	0m9.268s
sys 	0m29.264s
```

O tamanho dos ficheiros resultantes já está correto, comparado com a versão anterior. O tempo de execução desta versão é bastante mais lento do que as versões anteriores. Através da ferramenta `htop` podemos fazer uma análise mais profunda à execução do programa e verificamos que `./metanode 2` está a realizar um elevado número de escritas:
![HTOP](imgs/htop-v2.png)

Através da ferramenta `strace`, podemos verificar que o programa está a efetuar escritas nos ficheiros byte a byte, realizando um número bastante elevado de system calls por segundo:
```
...
write(3, "S", 1)                        = 1
write(3, "R", 1)                        = 1
write(3, "d", 1)                        = 1
write(3, "O", 1)                        = 1
write(3, "u", 1)                        = 1
write(3, "N", 1)                        = 1
write(3, "G", 1)                        = 1
....
```


### Version 3

Output da versão 3 (`time ./metanode 3`):
```
real	2m21.690s
user	2m17.815s
sys	    0m4.568s
```

A versão 3 do programa é ainda mais lenta que a versão 2, mas através de `time` verificamos que o tempo de execução do sistema não é elevado e a maioria do tempo perdido foi a executar computações. Estes dados permitem-nos supor que o problema poderá não estar na má utilização de chamadas ao sistema.

Aplicando o programa `strace`, verificamos que as chamadas ao sistema estão a ser utilizadas de forma correta. A ferramenta `htop` mostra-nos que existe um grande consumo de CPU, mas as escritas não são elevadas.

Através do comando `perf` podemos efetuar uma análise às funções utilizadas, e verificamos que a maioria do tempo de execução é "perdido" na função `random_string()`, mais precisamente na função que gera número aleatórios `rand()`:
![alt text](imgs/perf-report.png)

Podemos também efetuar uma análise gráfica:
![alt text](imgs/flamegraph.png)


### Version 4

> :warning: **Recomendação**: executar `sudo -k` para "limpar" os privilégios root

Output da versão 4 (`time ./metanode 4`):
```
real	0m3.676s
user	0m1.599s
sys 	0m1.412s
```
O tamanho dos ficheiros está correto (10 MB) e o tempo de execução não apresenta anomalias. Em certos casos, podemos verificar que o programa pede a password ao utilizar, como se estivesse a realizar alguma operação priviligiada internamente.

Através da ferramenta `journalctl` verificamos que existe um inicio de sessão realizado internamente pelo programa `metanode 4` e executa um comando "malicioso":
![alt text](imgs/journalctl.png)

Podemos analisar as chamadas ao sistema e procurar por execuções de programas através de `strace`:
```
strace -f -e trace=execve ./metanode 4
```
- A opção `-f` permite seguir processo filhos (`fork()`)

Encontramos a chamada ao sistema que procuravámos, efetuada por um processo filho:
```
...
[pid  4677] execve("/usr/bin/sudo", ["sudo", "echo", "malicious!"], 0x5a1cb316c968 /* 24 vars */) = 0
...
```

Esta versão de `metanode` evidencia um risco de segurança em programas. Nesta situação conseguimos reconhecer o problema através de `journalctl` porque se trata de um acesso com `sudo`. Se não for usado `sudo`, a ferramenta não identifica o inicio de sessão.
