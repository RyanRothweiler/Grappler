using UnityEngine;
using System.Collections;

public class Item : MonoBehaviour
{
	public ItemButton buttonIn;
	public int stackCount;
	public string itemName;
	public TextMesh stackText;
	public bool itemHeld;

	public void Start()
	{
		stackCount = 1;
	}

	public void Update()
	{
		if (itemHeld)
		{
			stackText.text = "" + stackCount;
		}
	}

	public virtual void Use() {}
}
