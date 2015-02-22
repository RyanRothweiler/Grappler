using UnityEngine;
using System.Collections;
using UnityEditor;

[CustomEditor(typeof(WorldGenerator))]
public class WorldGeneratorEditor : Editor 
{
	public override void OnInspectorGUI()
	{

		DrawDefaultInspector();

		if (GUILayout.Button("Generate World"))
		{
			WorldGenerator worldGenerator = (WorldGenerator)target;
			worldGenerator.GenerateWorld();
		}
	}
}