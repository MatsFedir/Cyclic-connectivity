# Cyclic-connectivity
Ročníkový projekt - Matsiutsia Fedir

Jadrom tohto projektu je algoritmus, ktorý na vstupe prijíma kubický graf a vracia jeho cyklický hranový rez. Samotná implementácia priamo vychádza z postupov opísaných v nasledujúcej vedeckej práci - "CC_alg_Kral" ktorá je tu ponechaná. Dôrazne sa odporúča prečítať si ju pred zobrazením kódu, pretože väčšina dôkazov je napísaná vo vnútri nej. V závislosti od veľkosti grafu dosahuje algoritmus časovú zložitosť O(n^3 log n) alebo lepšiu. 

V rámci výskumu má čitateľ za úlohu vybrať a napísať algoritmus toku na nájdenie ciest medzi stromami a napísať algoritmus na nájdenie cyklického rezu z daných stromov a ciest. Oba tieto algoritmy musia bežať v čase O(n). Tu poskytnem (nie úplný) dôkaz správnosti mojich dvoch algoritmov. Skutočnosť, že bežia v čase O(n), sa dá ľahko overiť pohľadom na ich implementáciu.


1.1. Návrh toku (Ford-Fulkersonov model) a dôkaz správnosti Pre riešenie problematiky toku som sa samostatne rozhodol adaptovať myšlienku Ford-Fulkersonovho (FF) modelu. Tento krok nielenže uľahčil pochopenie logiky toku v sieti, ale aj viditeľne zlepšil čas behu (runtime) algoritmu.  

1.2. Dôkaz správnosti toku: Nech G je graf s jednotkovými kapacitami hrán a N je jeho reprezentácia vo FF modeli. Ak v pôvodnom grafe existuje presne k hranovo disjunktných ciest, každá z týchto ciest dokáže preniesť tok veľkosti 1, čo znamená, že vo FF modeli bude existovať maximálny tok s veľkosťou k. Naopak, na základe Vety o celočíselnosti toku (Integrality Theorem) platí, že ak algoritmus vo FF modeli nájde maximálny tok veľkosti k, tento tok je možné exaktne rozložiť na presne k hranovo disjunktných ciest v pôvodnom grafe G. Týmto je zaručená ekvivalencia medzi hľadaním toku a hľadaním disjunktných ciest. 


2.1. Algoritmus hľadania cyklického rezu Logika a algoritmus pre samotné hľadanie rezu boli pôvodne navrhnuté v rámci konzultácií s AI asistentom (Large Language Model), pričom som tento návrh následne detailne upravil, implementoval do neho správny backtracking a optimalizoval ho pre potreby kubických grafov v tomto projekte. 

2.2. Dôkaz funkčnosti rezu: Algoritmus funguje na princípe postupného budovania stromov (Tv  a Tw) a prehľadávania reziduálneho grafu. Využíva stavový priestor hrán (tok = 0,1,2) na určenie povoleného smeru prechodu. Správnosť vyhľadávania je garantovaná dvoma faktormi: po prvé, udržiavanie poľa navštívených vrcholov (visited) striktne zabraňuje vzniku nekonečných cyklov; po druhé, precízny backtracking (nulovanie toku hrany pri návrate z neúspešnej rekurzívnej vetvy) zabezpečuje, že algoritmus "nepokazí" reziduálny graf slepými uličkami a systematicky preskúma všetky existujúce cesty. Ak stromy Tv  a množina cieľov A nájdu prienik, rez s istotou existuje a je korektne zaznamenaný. 
