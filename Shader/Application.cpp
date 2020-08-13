#include "Application.h"
#include "HeightMap.h"

Application* Application::s_pApp = NULL;

const int CAMERA_MAP = 0;
const int CAMERA_ROTATE = 1;
const int CAMERA_MAX = 2;







//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool Application::HandleStart()
{
	s_pApp = this;

	m_frameCount = 0.0f;

	m_bWireframe = true;
	m_pHeightMap = new HeightMap( "Resources/heightmap.bmp", 2.0f );

	m_cameraZ = 50.0f;
	m_rotationAngle = 0.f;

	m_reload = false;

	ReloadShaders();

	if (!this->CommonApp::HandleStart())
		return false;

	this->SetRasterizerState( false, m_bWireframe );

	m_cameraState = CAMERA_MAP;

	//setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO();

	bool test = ImGui_ImplWin32_Init(this->m_hWnd);

	test = ImGui_ImplDX11_Init(this->GetDevice(), this->GetDeviceContext());
	ImGui::StyleColorsDark();

	return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::HandleStop()
{
	// Shutdown
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	delete m_pHeightMap;

	this->CommonApp::HandleStop();
}



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::ReloadShaders()
{
	if( m_pHeightMap->ReloadShader() == false )
		this->SetWindowTitle("Reload Failed - see Visual Studio output window. Press F5 to try again.");
	else
		this->SetWindowTitle("Your Shader Here. Press F5 to reload shader file.");
}

void Application::HandleUpdate()
{
	if( m_cameraState == CAMERA_ROTATE )
		m_rotationAngle += .01f;

	if( m_cameraState == CAMERA_MAP || m_cameraState == CAMERA_ROTATE )
	{
		if (this->IsKeyPressed('Q'))
			m_cameraZ -= 2.0f;
		
		if (this->IsKeyPressed('A'))
			m_cameraZ += 2.0f;
	}

	
	static bool dbC = false;

	if (this->IsKeyPressed('C') )	
	{
		if( !dbC )
		{
			if( ++m_cameraState == CAMERA_MAX )
				m_cameraState = CAMERA_MAP;

			dbC = true;
		}
	}
	else
	{
		dbC = false;
	}


	static bool dbW = false;
	if (this->IsKeyPressed('W') )	
	{
		if( !dbW )
		{
			m_bWireframe = !m_bWireframe;
			this->SetRasterizerState( false, m_bWireframe );
			dbW = true;
		}
	}
	else
	{
		dbW = false;
	}


	if (this->IsKeyPressed(VK_F5))
	{
		if (!m_reload)
		{
			ReloadShaders();
			m_reload = true;
		}
	}
	else
		m_reload = false;

	if (this->IsKeyPressed(VK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	//Demo Text - utilising the ImGui library
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//create Demo test window
	ImGui::Begin("Shader Tutorial", NULL);
	ImGui::SetWindowPos(ImVec2(25.6f, 25.6f));
	ImGui::SetWindowSize(ImVec2(326.f, 300.f));

	if (ImGui::CollapsingHeader("Demo Description:", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("A real time 3d techniques for games");
		ImGui::Text("practical that is culmination of");
		ImGui::Text("the first few practicals, this one  ");
		ImGui::Text("focused on using a colour map and");
		ImGui::Text("multiple textures to texture the ");
		ImGui::Text("heightmap ");
		ImGui::Text("");
	}

	if (ImGui::CollapsingHeader("The Demo's control's", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (m_cameraState == CAMERA_MAP)
			ImGui::Text("The current camera state is: Map");
		else
			ImGui::Text("The current camera state is: Rotate");
		ImGui::BulletText("C - Change current camera state");
		ImGui::BulletText("W - Change to wireframe");
		ImGui::BulletText("ESC - Quit Application");
		ImGui::BulletText("Q - Zoom in");
		ImGui::BulletText("A - Zoom out");
	}

	ImGui::End();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::HandleRender()
{
	XMFLOAT3 vUpVector(0.0f, 1.0f, 0.0f);
	XMFLOAT3 vCamera, vLookat;

	switch (m_cameraState)
	{
	case CAMERA_MAP:
		vCamera = XMFLOAT3(sin(m_rotationAngle)*m_cameraZ, m_cameraZ / 4, cos(m_rotationAngle)*m_cameraZ);
		vLookat = XMFLOAT3(0.0f, 4.0f, 0.0f);
		break;
	case CAMERA_ROTATE:
		vCamera = XMFLOAT3(sin(m_rotationAngle)*m_cameraZ, m_cameraZ / 4, cos(m_rotationAngle)*m_cameraZ);
		vLookat = XMFLOAT3(0.0f, 4.0f, 0.0f);
		break;
	}

	XMMATRIX  matView;
	matView = XMMatrixLookAtLH(XMLoadFloat3(&vCamera), XMLoadFloat3(&vLookat), XMLoadFloat3(&vUpVector));

	XMMATRIX matProj;
	matProj = XMMatrixPerspectiveFovLH(float(XM_PI / 4), 2, 1.5f, 5000.0f);

	this->EnableDirectionalLight(1, XMFLOAT3(-1.f, -1.f, -1.f), XMFLOAT3(0.55f, 0.55f, 0.65f));

	this->SetViewMatrix(matView);
	this->SetProjectionMatrix(matProj);

	this->Clear(XMFLOAT4(0.3f, .3f, 4.f, 1.f));

	m_pHeightMap->Draw(m_frameCount);

	m_frameCount++;

	//assemble together draw data
	ImGui::EndFrame();
	ImGui::Render();

	//render draw data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int)
{
	Application application;

	Run(&application);

	return 0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
