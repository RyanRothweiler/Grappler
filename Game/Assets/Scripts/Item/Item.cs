using UnityEngine;
using System.Collections;

public class Item : MonoBehaviour
{
	public ItemButton buttonIn;
	public int stackCount;
	public string itemName;
	public bool itemHeld;
	
	public TextMesh nameText;
	public TextMesh stackText;

	public void Start()
	{
		stackCount = 1;
	}

	public void Update()
	{
		if (itemHeld)
		{
			stackText.text = "" + stackCount;
			nameText.gameObject.SetActive(true);
		}
		else
		{
			nameText.gameObject.SetActive(false);
		}
	}

	public virtual void Use() {}
}
