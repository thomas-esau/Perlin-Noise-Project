* PerlinModifiers mod in PerlinNoise -> nach PerlinMap. Jede Teilkarte ben�tigt nicht unbedingt eigene mod Datei.
	* mod als Argument in Methoden benutzen
* PerlinNoise2D_MEM in PerlinMap: L�sung f�r 
* Oneliner entweder in den Header verschieben oder in cpp
* Allocate Grid: i und j f�r size_x und size_y tauschen
* Grid Variable in PerlinNoise: Durch Vector ersetzen?
* getVector: Wenn Subsections durchlaufen werden, entsteht ein �berlauf.
* PerlinNoise2D: Trennung von Berechnung und Kartenlogik
* In PerlinNoise2D.cpp: SUBSECTIONS_X und Y statisch machen
* ObjectID -> ObjectNr umbenennung
* PerlinMap: constructMaps: Funktionsparameter �ndern: Welche Variable wird nur einmal ben�tigt, welche als Member?
*PerlinMap: Eindimensionaler Vector f�r <T> Pointer ausreichend.