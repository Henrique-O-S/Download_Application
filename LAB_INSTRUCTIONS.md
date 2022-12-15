## commandos para realizar no início:
- desconectar cabos ligados ao switch ou ao microtik preto
- systemctl restart networking (nos TUX todos)
- Echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts --> ignorar a ignorância dos broadcasts

Tux 2 - ifconfig 172.16.Y1.1
Tux 3 - ifconfig 172.16.Y0.1
Tux 4 - ifconfig 172.16.Y0.254

- Ligar E dos Tux ao switch e S do TUX2 ao control (passando primeiro pelo RS232)

- Correr commando impresso na mesa para dar restart à bridge (através do TUX2 ligado ao control)

## Implementar duas bridges num switch (3)

### Remover as portas associadas aos TUXs da bridge default
 - /interface bridge port remove [find interface=etherXX]

 ### Criar as duas bridges no switch

- /interface bridge add name=bridgeY0
- /interface bridge add name=bridgeY1

### Adicionar as portas associadas aos TUX no switch às respetivas bridges

- /interface bridge port add interface=etherXX bridge=bridgeYX

Verificação: 
- /interface bridge port print brief
- /interface bridge port print
- /interface bridge print

Nesta configuração não existe comunicação entre as duas bridges


## Configurar um router (3)

Ligar o eth1 da Tux4 ao switch

### Configurar o ip
- Tux 4 - ifconfig 172.16.Y1.253

### Remover a porta da bridge default
- /interface bridge port remove [find interface=etherXX]

### Adicionar à bridge Y1
- /interface bridge port add interface=etherXX bridge=bridgeY1

### Configuração
- Echo 1 > /proc/sys/net/ipv4/ip_forward --> apenas na TUX4 para dar enable ao IP forwarding

- Echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts --> ignorar a ignorância dos broadcasts

Adicionar rotas (`route -n` para visualizar) à TUX3 e TUX2 (respetivamente)

- route add default gw 172.16.40.254 —> eth0 da maquina 4
- route add default gw 172.16.41.253 —> eth1 da maquina 4

Desta forma já existe comunicação entre as duas bridges. Adicionamos gateways a cada uma das maquinas. Assim todos os pacotes que cada uma envia vão para essa gateway.

## Configurar um router comercial e implementar NAT (4)

### Passo 1

Ligar o microtik preto (Rc) ao PY1 (para dar internet) --> Eth1

Ligar o S0 do TUX2 ao Router MKTIK

- (router)/ip address add address=172.16.2.Y9/24 interface=ether1

Connectar o Eth2 do Rc ao switch (e adicionar na bridge Y1) --> para isto configurar o endereco ip

- (switch) /interface bridge port remove [find interface=etherXX] --> XX = port do switch onde está ligado o eth2 do Rc

- (switch) /interface bridge port add interface=etherXX bridge=bridgeY1

- (switch) /interface bridge port print brief --> verificar

- (router)/ip address add address=172.16.Y1.254/24 interface=ether2

Verificar os IPs

- (router) /ip address print

ROUTER --> S0 TUX2 ligado ao Router MKIK
SWITCH --> S0 ligado ao Control do Switch

### Passo 2

Verificar rotas com route -n

Caso nao estejam

- route add default gw 172.16.Y0.254 (no TUX3)

Adicionar Rc como default router para TUX2 e TUX3

- route add default gw 172.16.Y1.254 (no TUX2 e TUX4)

Adicionar rotas em TUX2 e Rc

- /ip route add 172.16.Y0.0/24 via 172.16.Y1.253

- (router) -> /ip route add dst-address=0.0.0.0/0 gateway=172.16.2.254 (ONly for lab i320)
- (router) -> /ip route add dst-address=172.16.Y0.0/24 gateway=172.16.Y1.253
- (router) -> /ip route print

apagar uma rota --> route del -net 172.16.10.0 gw 172.16.11.253 net mask 255.255.255.0

## DNS (5)

### Passo 1

- sudo nano /etc/resolv.conf
- nameserver 172.16.2.1







