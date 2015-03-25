using UnityEngine;
using System.Collections;

public class ItemButton : MonoBehaviour
{

	public Item itemHolding;
	public SpriteRenderer uiSprite;
	public TextMesh stackText;

	public SpriteRenderer controllerButtonIcon;
	public Sprite psButtonIcon;
	public Sprite xboxButtonIcon;

	public void Start()
	{
		if (God.instance.psControllerLayout)
		{
			this.controllerButtonIcon.sprite = psButtonIcon;
		}
		else
		{
			this.controllerButtonIcon.sprite = xboxButtonIcon;
		}
	}

	public void Use()
	{
		if (itemHolding != null)
		{
			itemHolding.Use();

			itemHolding.stackCount--;
			if (itemHolding.stackCount == 0)
			{
				PlayerController.instance.inventory.Remove(itemHolding);
				Destroy(itemHolding.gameObject);
				uiSprite.sprite = null;
				itemHolding = null;
			}
		}
		else
		{
			Debug.LogWarning("No item held in that slot.");
		}
	}

	public void Update()
	{
		if (itemHolding != null)
		{
			controllerButtonIcon.enabled = false;
			stackText.text = "" + itemHolding.stackCount;
		}
		else
		{
			controllerButtonIcon.enabled = true;
			stackText.text = "";
		}
	}

	public void Equip(Item newItem)
	{
		if (newItem.buttonIn != null)
		{
			newItem.buttonIn.uiSprite.sprite = null;
		}

		newItem.buttonIn = this;
		itemHolding = newItem;
		uiSprite.sprite = newItem.GetComponent<SpriteRenderer>().sprite;
	}
}
