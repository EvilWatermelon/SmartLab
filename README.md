# SmartLab

Das Living Lab ist ein IoT System, um ein Labor zu digitalisieren. Durch die Entwicklung verschiedener Komponenten kann eine Kommunikation zwischen den Objekten hergestellt werden.
Um die Kommunikation zu realisieren, wird ein Webserver aufgesetzt. Dadurch kann ein Netzwerk aufgebaut werden, welches die Kommunikation zwischen Stuhl und Licht gewährleistet.
Zudem ist durch offene REST Schnittstellen für eine einfache Möglichkeit gesorgt, das System mit zusätzlichen Komponenten  zu erweitern.

# ToDO

+ NFC Sensor verstärken
- Gui für das Anmdelen der ESP in Netzwerk
- Gui für Farbauswahl von Stühlen
- Lichtsensor implementieren
- LED-Strip in die Tür
- Geschwindigkeitsoptimierung 


\section{HARDWARE-ARCHITEKTUR}

Die Hardware Architektur besteht aus NFC-Tags zum Clustern des Raums. Damit lässt sich die Position eines Stuhls bestimmen. Zu jeder Flächenposition im Raum gibt es eine Glühbirne, die aufleuchtet, sobald ein besetzter Stuhl über die NFC-Tags auf die Fläche rollt. Zum Erkennen eines besetzten Stuhls ist immer ein Drucksensor eingebaut. Der Sensor erkennt das ansteigende Gewicht und gibt ein Signal, dass der Stuhl besetzt ist. Jeder Stuhl hat einen RFID Reader, angebaut an einem NodeMCU. Der NodeMCU gibt die Position und die Bestätigung, das ein Stuhl besetzt ist, an den Raspberry Pi weiter, auf dem ein Webserver läuft. Das System ist in einem eigenen Netzwerk, damit eine Datenübertragung problemlos stattfindet. 
