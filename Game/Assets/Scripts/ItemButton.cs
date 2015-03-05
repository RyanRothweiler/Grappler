using UnityEngine;
using System.Collections;

public class ItemButton : MonoBehaviour
{

	public Item itemHolding;
	public SpriteRenderer uiSprite;

	public void Use()
	{
		if (itemHolding != null)
		{
			itemHolding.Use();
		}
		else
		{
			Debug.LogWarning("No item held in that slot.");
		}

		if (itemHolding == null)
		{
			uiSprite.sprite = null;			
		}
	}

	public void Equip(Item newItem)
	{
		if (itemHolding)
		{
			//unequip the item here
		}

		newItem.buttonIn = this;
		itemHolding = newItem;
		uiSprite.sprite = newItem.GetComponent<SpriteRenderer>().sprite;
	}
}
