[ComponentEditorProps(category: "GameScripted/TrainWreck/Area Mesh", description: "")]
class TW_RuntimeAreaMeshClass : SCR_BaseAreaMeshComponentClass {};

class TW_RuntimeAreaMesh : SCR_BaseAreaMeshComponent
{
	[Attribute("1")]
	protected float m_fRadius;
	
	override float GetRadius()
	{
		return m_fRadius;
	}
	void SetRadius(float newRadius) {
		m_fRadius = newRadius;
		GenerateAreaMesh();
	}
	override void EOnInit(IEntity owner)
	{
		GenerateAreaMesh();
	}
};