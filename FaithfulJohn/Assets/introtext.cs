using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class introtext : MonoBehaviour {
	Text test;
	int count = 0;
	// Use this for initialization
	void Start () {
		test = GetComponent <Text> ();
		
	}
	
	// Update is called once per frame
	void Update () {
		if (count == 100) {
			test.text = "We are on the Voyager III, a spacecraft built to save mankind.";
		}
		if (count == 360) {
			test.text = "I am a Robotic Android Vision-guided E-companioN or RAVEN for short.";
		}
		if (count == 760) {
			test.text = "Together, we've been sent to a distant planet to prevent an alien takeover.";
		}
		if (count == 1200) {
			test.text = "In a few moments, we will land on the planet and begin our mission.";
		}
		if (count == 1560) {
			test.text = "The fate of the human race is in your hands. Good luck.";
		}
		if (count == 1810) {
			test.text = "";
		}
		if (count == 1900) {
			Application.LoadLevel("CityScene");
				}
		count++;
	}
}
