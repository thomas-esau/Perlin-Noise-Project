* PerlinModifiers mod in PerlinNoise -> nach PerlinMap. Jede Teilkarte benötigt nicht unbedingt eigene mod Datei.
	* mod als Argument in Methoden benutzen
* PerlinNoise2D_MEM in PerlinMap: Lösung für 
* Oneliner entweder in den Header verschieben oder in cpp
* Allocate Grid: i und j für size_x und size_y tauschen
* Grid Variable in PerlinNoise: Durch Vector ersetzen?
* getVector: Wenn Subsections durchlaufen werden, entsteht ein Überlauf.
* PerlinNoise2D: Trennung von Berechnung und Kartenlogik
* In PerlinNoise2D.cpp: SUBSECTIONS_X und Y statisch machen
* ObjectID -> ObjectNr umbenennung
* PerlinMap: constructMaps: Funktionsparameter ändern: Welche Variable wird nur einmal benötigt, welche als Member?
*PerlinMap: Eindimensionaler Vector für <T> Pointer ausreichend.