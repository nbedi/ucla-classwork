using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class text : MonoBehaviour {
	Text test;
	int count = 0;
	// Use this for initialization
	void Start () {
		test = GetComponent <Text> ();

	}
	
	// Update is called once per frame
	void Update () {
		if (count == 100) {
			test.text = "Our first task is to steal a rare element from an alien scientist's home.";
		}
		if (count == 260) {
			test.text = "I have programmed the location into your sight unit.";
		}
		if (count == 460) {
			test.text = "Your eyes will now see the house glowing green.";
		}
		if (count == 700) {
			test.text = "Your body has also been programmed to be invisible to the aliens so you are fine walking around.";
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
