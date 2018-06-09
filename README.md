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

# HARDWARE-ARCHITEKTUR

Die Hardware Architektur besteht aus NFC-Tags zum Clustern des Raums. Damit lässt sich die Position eines Stuhls bestimmen. Zu jeder Flächenposition im Raum gibt es eine Glühbirne, die aufleuchtet, sobald ein besetzter Stuhl über die NFC-Tags auf die Fläche rollt. Zum Erkennen eines besetzten Stuhls ist immer ein Drucksensor eingebaut. Der Sensor erkennt das ansteigende Gewicht und gibt ein Signal, dass der Stuhl besetzt ist. Jeder Stuhl hat einen RFID Reader, angebaut an einem NodeMCU. Der NodeMCU gibt die Position und die Bestätigung, das ein Stuhl besetzt ist, an den Raspberry Pi weiter, auf dem ein Webserver läuft. Das System ist in einem eigenen Netzwerk, damit eine Datenübertragung problemlos stattfindet. 

Alternativ zur Client-Server Architektur hätte auch eine Peer-to-Peer Architektur verwendet werden können, indem die einzelnen Geräte direkt miteinander kommunizieren. Jedoch wäre dadurch die Erweiterung durch verschiedene Komponenten tendenziell schwerer. Durch die Client-Server Architektur muss nicht jedes Gerät jedes andere Gerät kennen, sondern alle Geräte müssen ausnahmslos den Server kennen beziehungsweise der Server die Geräte, die er ansprechen muss. Sollte folglich ein weiterer Aktor dazukommen, müsste dies nur im Server implementieren werden und nicht auf allen vorherigen Objekten. Wenn jedoch nur ein weiteres Sensorobjekt, wie ein Stuhl dazu kommt, muss dies nur auf dem Sensorobjekt selbst berücksichtigen, aber nicht auf anderen Geräten.

# SOFTWARE-ARCHITEKTUR

Die Software Architektur besteht aus C Programmen und einem in Javascript geschriebenen Webserver.
Die Stühle senden ihre Position und ob sie besetzt sind über ein Programm, welches sich auf dem NodeMCU befindet, an den Webserver. Dieser wird auf dem Raspberry Pi ausgeführt. Der Webserver nimmt die Position der Stühle auf und sendet an die entsprechende Glühbirne ein Signal zum aufleuchten.

<h3>Webserver</h3>

Der Webserver ist das "Kernstück". Sobald er gestartet wird liest er die Konfigurationsdaten sowie die IPs der einzelnen Geräte aus, sodass er mit diesen Kommunizieren kann. Um das Projekt mit weiteren Geräten zu erweitern, können in den JSON Datein zusätzliche Einträge hinzugefügt werden. Für eine bessere Übersicht sorgt eine GUI welche über "(Ip-Adresse des Raspberry)/lamps" und "(Ip-Adresse des Raspberry)/chairs" mit einem Browser aufgerufen werden. 

Der Server bekommt von allen Sensoren Nachrichten mit dem Inhalt der Messwerte. In der aktuellen Version ist dies nur die ID eines ausgelesenen Tags und der Zustand eines Stuhls, also ob jemand auf ihm sitzt oder nicht. Eine Implementierung von weiteren Schnittstellen ist jedoch einfach gestaltet, sodass auch andere Sensoren wie ein Lichtsensor zusätzliche Werte an den Server senden kann. Die gemessenen Werte werden bei Veränderungen in den ".json" Datein gespeichert.

Um die Position der NFC-Tags zu ermitteln, wird das Json-Objekt mit der ausgelesen ID aus der Liste aller Tag-Objekte gesucht. In diesem Objekt befindet sich auch die vorher festgelegte Position. Eine vorherige Lösung bei der die Position in dem Speicher der NFC-Tags gespeichert wird, wurde verworfen da der genutzte RFID-Sensor nur die ID der NFC-Tags auslesen kann und NFC-Tags nur in einer höheren Preisklasse zu finden sind.

Um zu entscheiden ob ein Ansprechgerät wie z.B. eine Lampe oder ein Ventilator nun aktiviert werden soll, wird dazu die Entfernung zwischen der Position des NFC-Tags und der Positon des Ansprechgeräts berechnet. Wenn sich das Gerät nun innerhalb des Kreises, dessen Radius in dem Ansprechgerät-Json-Objekt gespericht ist, liegt also die Entfernung nahe genug beieinander ist, wird dieses aktiviert. Die Möglichkeit unterschiedliche Entfernungen einzustellen wurde implementiert, da davon ausgegangen werden kann, das Lampen verschiedene Helligkeiten besitzen oder auch entsprechend ihrer Position ein anderer Aktivierungsradius sinnvoll ist.

Auch andere Formen als ein Kreis zur Aktivierung der Geräte, wie beispielsweise die eines Kegels, ist relativ einfach zu implementieren, da die Überprüfung in einer eigenen Funktion ausgeführt wird. So können für andere Formen eigene Funktionen geschrieben werden, welche dann entsprechend des Gerätes aufgerufen werden.

Im nächsten Schritt werden nun alle Geräte deren Status sich verändert hat über eine REST Schnittstelle aufgerufen und der neue Status gesendet. Diese Änderungen werden ebenfalls in der "Ansprechgerät.json" Datei gespeichert, so das bei einem Serverausfall der aktuelle Status immernoch gesichert ist. Hier kann es jedoch zu Problemen kommen falls der Status eines Gerätes sich nicht über das System ändert. Sollte beispielsweise ein Licht über einen Schalter aktivert werden, würde sich der Status auf der Serverseite nicht ändern. Somit würde die Lampe auch nicht deaktivert werden, wenn eine Person vom Stuhl aufsteht, da der Server bereits gespeichert hat, das diese deaktivert ist und somit kein Signal sendet, das sie deaktivert werden soll. Eine Schnittstelle, welche dem Server mitteilt, dass der Status eines Gerätes sich verändert hat, wäre eine sinnvolle Weiterentwicklung. 


<h3>Struktur der JSON Dokumente</h3>
<table>
<caption>Chairs</caption>
<thead>
<tr class="header">
<th style="text-align: center;">Atr.Name</th>
<th style="text-align: center;">Typ</th>
<th style="text-align: center;">Beispiel</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td style="text-align: center;">ID</td>
<td style="text-align: center;">Mac.Addr</td>
<td style="text-align: center;">68:C6:3A:9F:59:0C</td>
</tr>
<tr class="even">
<td style="text-align: center;">State</td>
<td style="text-align: center;">bool</td>
<td style="text-align: center;">0 oder 1</td>
</tr>
<tr class="odd">
<td style="text-align: center;">Position</td>
<td style="text-align: center;">x,y</td>
<td style="text-align: center;">{&quot;x&quot;: &quot;400&quot;,&quot;y&quot;: &quot;100&quot;}</td>
</tr>
<tr class="even">
<td style="text-align: center;">color</td>
<td style="text-align: center;">r,g,b</td>
<td style="text-align: center;">{&quot;r&quot;: 255,&quot;g&quot;: 0,&quot;b&quot;: 230}</td>
</tr>
</tbody>
</table>


<dl>
<dt>1.</dt>
<dd><p>Als ID eines Stuhls wird die Mac Adresse des NodeMCU verwendet, welcher an dem Stuhl befestigt ist.</p>
</dd>
<dt>2.</dt>
<dd><p>Der Name State gibt den Status an, ob der Stuhl besetzt ist oder nicht.</p>
</dd>
<dt>3.</dt>
<dd><p>Die Position gibt die x und y Koordinaten des jeweiligen Clusters an, auf dem sich ein Stuhl befindet.</p>
</dd>
<dt>4.</dt>
<dd><p>Ist auf einer Position eine Philips Hue, so zeigt der color Wert die Farbe an mit der die Glühbirne den Bereich beleuchten soll.</p>
</dd>
</dl>

<table>
<caption>Ansprechgerät</caption>
<thead>
<tr class="header">
<th style="text-align: center;">Atr.Name</th>
<th style="text-align: center;">Typ</th>
<th style="text-align: center;">Beispiel</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td style="text-align: center;">ID</td>
<td style="text-align: center;">Int</td>
<td style="text-align: center;">135</td>
</tr>
<tr class="even">
<td style="text-align: center;">ip</td>
<td style="text-align: center;">Ip-Address</td>
<td style="text-align: center;">192.168.2.79</td>
</tr>
<tr class="odd">
<td style="text-align: center;">Type</td>
<td style="text-align: center;">String</td>
<td style="text-align: center;">Hue</td>
</tr>
<tr class="even">
<td style="text-align: center;">name</td>
<td style="text-align: center;">String</td>
<td style="text-align: center;">Four</td>
</tr>
<tr class="odd">
<td style="text-align: center;">hueUsername</td>
<td style="text-align: center;">String</td>
<td style="text-align: center;">CKz4-mVVsU8</td>
</tr>
<tr class="even">
<td style="text-align: center;">status</td>
<td style="text-align: center;">bool(String)</td>
<td style="text-align: center;">true oder false</td>
</tr>
<tr class="odd">
<td style="text-align: center;">position</td>
<td style="text-align: center;">x,y</td>
<td style="text-align: center;">{&quot;x&quot;: &quot;400&quot;,&quot;y&quot;: &quot;100&quot;}</td>
</tr>
<tr class="even">
<td style="text-align: center;">activationRadius</td>
<td style="text-align: center;">Int</td>
<td style="text-align: center;">200</td>
</tr>
</tbody>
</table>
<dl>
<dt>1.</dt>
<dd><p>Eindeutige Zuweisung der Glühbirnen durch eine ID.</p>
</dd>
<dt>2.</dt>
<dd><p>Eine statische IP Adresse, um zu entscheiden, welcher Host damit aufgerufen wird.</p>
</dd>
<dt>3.</dt>
<dd><p>Type unterscheidet zwischen der Philips Hue und dem Sonoff.</p>
</dd>
<dt>4.</dt>
<dd><p>name bennent die Glühbirnen als String.</p>
</dd>
<dt>5.</dt>
<dd><p>hueUsername ist ein zufällig generierter Name, um dem User der die Bridge nutzen möchte Rechte zu geben.</p>
</dd>
<dt>6.</dt>
<dd><p>status zeigt ob eine Glühbirne an einer Position aufleuchtet oder nicht.</p>
</dd>
<dt>7.</dt>
<dd><p>Die Ansprechgeräte speichern ihre eigene Position über die x und y Koordinaten.</p>
</dd>
<dt>8.</dt>
<dd><p>activationRadius bestimmt die Größe eines Clusters in der eine Glühbirne maximal leuchten darf.</p>
</dd>
</dl>

<table>
<caption>NFC-Tags</caption>
<thead>
<tr class="header">
<th style="text-align: center;">Atr.Name</th>
<th style="text-align: center;">Typ</th>
<th style="text-align: center;">Beispiel</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td style="text-align: center;">ID</td>
<td style="text-align: center;">Int</td>
<td style="text-align: center;">884439F50</td>
</tr>
<tr class="even">
<td style="text-align: center;">name</td>
<td style="text-align: center;">String</td>
<td style="text-align: center;">Four</td>
</tr>
<tr class="odd">
<td style="text-align: center;">position</td>
<td style="text-align: center;">x,y</td>
<td style="text-align: center;">{&quot;x&quot;: &quot;400&quot;,&quot;y&quot;: &quot;100&quot;}</td>
</tr>
</tbody>
</table>
<dl>
<dt>1.</dt>
<dd><p>Die NFC-Tags haben eine vom Hersteller eindeutig vergebene ID. Diese als ID zu verwenden hat den Vorteil, dass die Tags nicht beschrieben werden müssen bevor sie verwendet werden können. Sollte eine Tag-ID gelesen werden wird Automatisch ein neuer eintrag in dem JSON-Dokument gemacht.</p>
</dd>
<dt>2.</dt>
<dd><p>name stellt den Namen einer Glühbirne dar.</p>
</dd>
<dt>3.</dt>
<dd><p>position gibt die x und y Koordinaten eines NFC-Tags wieder.</p>
</dd>
</dl>

<table>
<caption>User List</caption>
<thead>
<tr class="header">
<th style="text-align: center;">Atr.Name</th>
<th style="text-align: center;">Typ</th>
<th style="text-align: center;">Beispiel</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td style="text-align: center;">ID</td>
<td style="text-align: center;">Int</td>
<td style="text-align: center;">2</td>
</tr>
<tr class="even">
<td style="text-align: center;">color</td>
<td style="text-align: center;">r,g,b</td>
<td style="text-align: center;">{&quot;r&quot;: 0,&quot;g&quot;: 0,&quot;b&quot;: 230}</td>
</tr>
</tbody>
</table>
<dl>
<dt>1.</dt>
<dd><p>ID zum Identifizieren eines Users.</p>
</dd>
<dt>2.</dt>
<dd><p>color ist die Standardfarbe die der Nutzer sich aussuchen kann, welche bei RGB-Lampen angezeigt werden soll, wenn er sich diesen nähert.</p>
</dd>
</dl>


# INSTALLATIONSANLEITUNG

<h2 id="hardware">Hardware</h2>
<h3 id="philips-hue">Philips Hue</h3>
<dl>
<dt>1.</dt>
<dd><p>Für die Philips Hue ist es von Vorteil am Anfang die App auf dem Smartphone herunterzuladen. Damit können erste Voreinstellungen getroffen werden. Was mit der App eingestellt werden sollte, ist in der Software-Anleitung der Philips Hue beschrieben.</p>
</dd>
<dt>2.</dt>
<dd><p>Als erstes sollte die Philips Hue Bridge angeschlossen werden. Dazu wird das Netzwerkkabel der Hue Bridge genommen und in einen freien Slot des Routers gesteckt.</p>
</dd>
<dt>3.</dt>
<dd><p>Danach kann die Bridge an den Strom geschlossen werden.</p>
</dd>
<dt>4.</dt>
<dd><p>Um zu sehen ob alles funktioniert hat, müssen nun alle drei LEDs auf der Bridge aufleuchten.</p>
</dd>
<dt>5.</dt>
<dd><p>Als nächstes wird eine E27 Glühbirnenfassung genommen und die Glühbirne wird in die Fassung geschraubt.</p>
</dd>
<dt>6.</dt>
<dd><p>Anschließend kann getestet werden ob die Glühbirnen funktionieren. Die Glühbirnen lassen sich wie eine klassische Glühbirne über einen Lichtschalter an- und ausschalten.</p>
</dd>
<dt>7.</dt>
<dd><p>(Optional) Sollten die Glühbirnen länger geleuchtet haben und sollen wieder herausgeschraubt werden, sollte ca. zwei Minuten abgewartet werden, da sich diese schnell erhitzen.</p>
</dd>
<dt>8.</dt>
<dd><p>Nun kann getetestet werden ob die Glühbirnen über die Bridge angesteuert werden können.</p>
</dd>
<dt>9.</dt>
<dd><p>Dies lässt sich durch die Hue App, die API oder das eigene Programm testen. Am einfachsten ist wie bei Punkt 1 die App zu nutzen.</p>
</dd>
<dt>10.</dt>
<dd><p>Zum weiteren Testen geht es weiter in der Software-Anleitung der Philips Hue.</p>
</dd>
</dl>

<h2 id="software">Software</h2>
<h3 id="arduinoide">ArduinoIDE</h3>
<dl>
<dt>1.</dt>
<dd><p>Um die C-Programme zu verwenden wird die ArduinoIDE benötigt. Diese kann unter den folgenden Links für Windows, Mac oder Linux heruntergeladen werden:</p>
<pre><code>https://www.arduino.cc/
en/Main/Software
 </code></pre>
</dd>
<dt>2.</dt>
<dd><p>Ist die IDE installiert, gibt es vor dem benutzen einige Einstellungen in der Umgebung die vorgenommen werden müssen, da sonst auf dem NodeMCU kein Programm hochgeladen werden kann.</p>
</dd>
<dt>3.</dt>
<dd><p>Unter dem Reiter &quot;Datei&quot; wird der Unterpunkt &quot;Voreinstellungen&quot; ausgewählt. Dort wird in Textzeile zu &quot;Zusätzliche Boardverwalter-URLs:&quot; folgenden Link eingefügt:</p>
<pre><code>http://arduino.esp8266.com/
stable/package_esp8266com_index.json
 </code></pre>
</dd>
<dt>4.</dt>
<dd><p>Als letzte Einstellung muss unter dem Reiter &quot;Werkzeuge&quot; den Punkt &quot;Board [Name]&quot; selektiert werden. In dem Folgemenü wird nach &quot;ESP8266&quot; gesucht. Das Paket an erster Stelle muss nun installiert werden.</p>
</dd>
<dt>5.</dt>
<dd><p>(Optional) Sollen Bibliotheken für das Programm verwendet werden, gibt es unter dem Reiter &quot;Sketch&quot; den Punkt &quot;Bibliothek einbinden&quot;. Hier ist möglich, Bibliotheken herunterzuladen und dem Projekt hinzuzufügen über &quot;ZIP Dateien&quot;. Außerdem sollte die Upload Geschwindigkeit bei 115200 liegen, da das Programm beim Hochladen sonst Probleme verursachen kann. Wenn kein COM Port zur Verfügung stehen, liegt es oft an einem fehlenden Treiber des Boards, welcher extern installiert werden muss.</p>
</dd>
</dl>

<h3 id="github-projekt">Github Projekt</h3>
<dl>
<dt>1.</dt>
<dd><p>Auf</p>
<pre><code>https://github.com/
EvilWatermelon/SmartLab
 </code></pre>
<p>sind alle Programme zur Verfügung gestellt, die für dieses Projekt benötigt werden.</p>
</dd>
<dt>2.</dt>
<dd><p>Zuerst ist es wichtig sich einen neuen Ordner lokal auf einem PC zu erstellen. Darin kann über die Git Bash in Windows oder Linux mit dem Terminal das Projekt geklont werden.</p>
</dd>
<dt>3.</dt>
<dd><p>Über den Befehl</p>
<pre><code>git clone 
https://github.com/
EvilWatermelon/SmartLab.git
 </code></pre>
<p>können die entsprechenden Programme geklont werden. Wahlweise kann das Projekt auch als ZIP heruntergeladen werden.</p>
</dd>
<dt>4.</dt>
<dd><p>Nun können die entsprechenden Programme auf den jeweiligen Entwicklerboards mit der ArduinoIDE hochgeladen werden.</p>
</dd>
</dl>

<h3 id="server">Server</h3>
<dl>
<dt>1.</dt>
<dd><p>Um den Webserver aufsetzen zu können wird NodeJS benötigt. Dazu kann das Terminal auf dem Raspberry Pi geöffnet werden.</p>
</dd>
<dt>2.</dt>
<dd><p>Um NodeJs zu installieren müssen nun die folgenden Befehle eingegeben werden:</p>
<pre><code>curl -sL https://deb.nodesource.com/
setup_10.x | sudo -E bash -
 </code></pre>
<pre><code>sudo apt-get install -y nodejs
 </code></pre>
</dd>
<dt>3.</dt>
<dd><p>Um den Webserver auf dem Raspberry Pi auszuführen ist es nötig, das Projekt in einen Ordner auf dem Raspberry Pi zu klonen. Wurde dies durchgeführt, kann der folgende Befehl über das Terminal im Ordner ausgeführt werden:</p>
<pre><code>npm install
 </code></pre>
<p>Die nötigen Frameworks sind in der Package.json vermerkt, sodass der oben gezeigte Befehl auch diese zusätzlich installiert.</p>
</dd>
<dt>4.</dt>
<dd><p>Da es bei dem Port 80 zu Problemen bei Aufrufen des Servers gibt, ist eine Portweiterleitung nötig, welche mit dem folgendem Befehl eingerichtet wird:</p>
<pre><code>sudo iptables -t nat -A PREROUTING 
-p tcp --dport 80 -j REDIRECT 
--to-port 8080</code></pre>
</dd>
<dt>5.</dt>
<dd><p>Sobald die Installation abgeschlossen ist, kann mit dem Befehl:</p>
<pre><code>node index.js</code></pre>
<p>der Server gestartet werden. Bei einem erfolgreichen Start sollte die Rückmeldung &quot;Example app listening on port 80!&quot; in dem Terminal erscheinen.</p>
</dd>
</dl>

<h3 id="sonoff-4ch">Sonoff 4CH</h3>
<dl>
<dt>1.</dt>
<dd><p>Um den Sonoff 4CH zu programmieren wird ein Programmierer für den ESP8285 benötigt. Für dieses Projekt wird der &quot;FTDI FTD1232&quot; empfohlen. Mit der ArduinoIDE kann das entsprechende Programm, welches im folgenden Git Repository zu finden ist, hochgeladen werden</p>
<pre><code>https://github.com/
EvilWatermelon/SmartLab</code></pre>
</dd>
<dt>2.</dt>
<dd><p>Als erstes muss das Gehäuse des Sonoff 4CH geöffnet werden, um an die 5 Seriellen Pins zu kommen.</p>
<p>[htb] <img src="Bild4" title="fig:" alt="Sonoff 4CH mit offenem Gehäuse" width="220" /></p>
</dd>
<dt>3.</dt>
<dd><p>Die Pins werden dann mit dem ESP Programmierer verbunden. Hierbei sollte darauf geachtet werden, dass der Programmierer von 5V auf 3.3V umgestellt wurde. Ansonsten besteht die Gefahr, dass der eingebaute ESP kaputt geht und damit nicht mehr benutzbar ist. Die Verbindung mit dem Sonoff 4Ch ist in der folgenden Liste zu finden:</p>
<pre><code>Sonoff : Programmierer
GND    : GND
RX     : TX
TX     : RX
VCC33  : VCC
IO2    : GND</code></pre>
<p>Indem der IO2 Pin an den GND angeschlossen wird oder man den Knopf von IO0 drückt während der ESP mit dem Programmer verbunden wird, wird der Programmiermodus des Sonoff 4CH eingeschaltet. Über die Arduino IDE kann nun den Code aus dem GIT hochgeladen werden.</p>
</dd>
<dt>4.</dt>
<dd><p>Nachdem der Code auf den Sonoff 4CH geladen wurde, wird der Programmierer wieder entfernt und das Gerät wird an einen Stromkreis angeschlossen. Jeweils ein Anschluss ist für den Input des Stroms und die anderen 4 Anschlüsse sind für die Geräte bzw. Stromkreise, die angesteuert werden sollen.</p>
</dd>
<dt>5.</dt>
<dd><p>Das Gerät ist nun einsatzbereit. Zuerst sollte sich der Nutzer mit einem anderen Gerät, mit dem vom Sonoff 4CH erstellten WLan, verbinden:</p>
<pre><code>SSID: ESP_Config
Password: password</code></pre>
<p>Auf der folgenden Webseite kann nun das WLan Netz mit welchem sich der Sonoff 4CH verbinden soll, konfiguriert werden.</p>
<pre><code>192.168.4.2/config </code></pre>
<p>Anschließend ist das Gerät einsatzbereit.</p>
</dd>
</dl>

<h3 id="raspberry-pi">Raspberry Pi</h3>
<dl>
<dt>1.</dt>
<dd><p>Für den Webserver wird ein Raspberry Pi genutzt. Das Betriebssystem ist für dieses Projekt Raspbian. Um den Webserver auf dem Raspberry Pi zu nutzen wird NodeJS installiert. Wie es installiert wird, ist in der Software-Anleitung im Server Teil zu finden.</p>
</dd>
<dt>2.</dt>
<dd><p>Der Raspberry Pi kann über SSH per Terminal oder PuTTy genutzt werden oder über einen Monitor, Tastatur und Maus. Für diese Anleitung wird kein SSH benötigt.</p>
</dd>
<dt>3.</dt>
<dd><p>Der Monitor kann nur über HDMI angeschlossen werden. Geht dies nicht, hat der Raspberry Pi auch USB Schnittstellen, an welche ein Adapter angeschlossen werden kann. Wichtig ist, dass das HDMI Kabel vor dem Starten des Raspberry Pis angeschlossen sein muss, ansonsten ist die Bildschirmauflösung zu niedrig.</p>
</dd>
<dt>4.</dt>
<dd><p>Die Tastatur und Maus werden an die USB Schnittstellen angeschlossen.</p>
</dd>
<dt>5.</dt>
<dd><p>Für das Betriebssystem wird eine SD-Karte benutzt. Wie das Betriebssystem installiert wird, sollte es noch nicht vorinstalliert sein, wird in der offiziellen <a href="https://projects.raspberrypi.org/en/projects/noobs-install">&quot;Installing Raspbian with NOOBS&quot;</a> Anleitung beschrieben.</p>
</dd>
<dt>6.</dt>
<dd><p>Ist das Betriebssystem auf der SD-Karte, kann diese in den SD-Kartenslot des Raspberry Pis geschoben werden.</p>
</dd>
<dt>7.</dt>
<dd><p>Der Raspberry Pi besitzt keinen Power Schalter. Um diesen nun zu starten, muss der Stromstecker des Raspberry Pis in die Steckdose gesteckt werden.</p>
</dd>
</dl>

<h3 id="stühle">Stühle</h3>
<dl>
<dt>1.</dt>
<dd><p>Damit die Stühle erkennen, das sie besetzt sind, muss als erstes einen Drucksensor verbaut werden. Dieser ist unter dem Sitzpolster verbaut. Dafür muss der Stoffbezug gelöst und der Sensor unter die Schaumstofffüllung geleget werden. Die im Vorfeld angelöteten Kabel werden durch eine Öffnung geführt, sodass der Sensor später an den NodeMCU angeschlossen werden kann.</p>
</dd>
<dt>2.</dt>
<dd><p>Als nächstes wird auf die Seite des 3D-Druckgehäuses mit dem viereckingen Kasten der NodeMCU gesteckt. Anschließend werden die Kabel angeschlossen und durch die vorhandene Öffnung des Stuhls geführen.</p>
</dd>
<dt>3.</dt>
<dd><p>Nun kann die soeben verkabelte Hälfte des 3D-Druckgehäuses mit der anderen Hälfte am unteren Teil des Stuhls zusammenfügt und festschraubt werden.</p>
</dd>
<dt>4.</dt>
<dd><p>Nachdem das Gehäuse befestigt wurde, kann in das untenliegende Einschubfach der RFID-Reader gelegt und mit den entsprechenden Kabeln verbunden werden.</p>
</dd>
<dt>5.</dt>
<dd><p>Bevor der Stuhl einsatzfähig ist, muss der Programmcode auf den NodeMCU geladen werden. Hierfür wird der NodeMCU mit einem MicroUSB Kabel an einen Computer angeschlossen und mithilfe der Arduino IDE wird der entsprechenden Programmcode auf das Gerät geladen.</p>
</dd>
<dt>6.</dt>
<dd><p>Als Stromqulle für die Geräte die an dem Stuhl befestigt sind, wird eine Powerbank benötigt. Für diese befindet sich eine Halterung am 3D-Gehäuse. Da Powerbanks jedoch unterschiedliche größen haben, muss hier darauf geachtet werden, dass das Gehäuse zu den Powerbanks passt. Wenn keine der in diesem Projekt benutzten Powerbanks vorhanden ist, muss das 3D-Modell an die andere Form der genutzten Powerbank angepasst werden.</p>
</dd>
<dt>7.</dt>
<dd><p>Wenn die Powerbank mit dem NodeMCU verbunden ist, startet das Gerät und kann sich mit anderen Gerät, über den vom NodeMCU erstellten WLan-Hotspot verbinden.</p>
<pre><code>SSID: ESP_Config
Password: password</code></pre>
<p>Auf der folgenden Webseite, kann nun das WLan Netz, mit welchem sich der NodeMCU verbinden soll, festgelegt werden.</p>
<pre><code>192.168.4.2/config </code></pre>
<p>Anschließend ist der Stuhl einsatzbereit.</p>
</dd>
</dl>
