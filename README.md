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


<h2 id="struktur-der-json-dokumente">Struktur der JSON Dokumente</h2>
<p><span id="list_of_Objects" label="list_of_Objects">[list_of_Objects]</span></p>
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
