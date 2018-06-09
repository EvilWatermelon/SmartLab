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
