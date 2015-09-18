using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class text2 : MonoBehaviour {
	Text test;
	int count = 0;
	// Use this for initialization
	void Start () {
		test = GetComponent <Text> ();
		
	}
	
	// Update is called once per frame
	void Update () {
		if (count == 100) {
			test.text = "Well done John. Now we have to go to the tallest building in the city.";
		}
		if (count == 260) {
			test.text = "Once again, I have programmed the location into your sight unit.";
		}
		if (count == 460) {
			test.text = "Your eyes will now see the building glowing red.";
		}
		if (count == 700) {
			test.text = "Your body is still programmed to be invisible to the aliens so you are fine walking around.";
		}
		if (count == 1060) {
			test.text = "Good luck.";
		}
		if (count == 1210) {
			test.text = "";
		}
		count++;
	}
}
