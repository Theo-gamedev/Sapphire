﻿#include "TestLighting.h"

TestLighting::TestLighting(sph::Application* const _app)
	: Layer("TestLighting")
	, m_app(_app)
	, m_lightData()
	, m_ambiantLightColor(0.1f, 0.1f, 0.1f)
	, m_tileMapData()
	, m_postFXProjection(0.0f)
{
}

TestLighting::~TestLighting()
{
}

void TestLighting::OnAttach()
{
	// Camera
	glm::vec2 halfSize = m_app->GetWindow().GetSize() / 2.0f;
	m_camera = sph::OrthographicCamera::Create(-halfSize.x, halfSize.x, -halfSize.y, halfSize.y);
	m_cameraController = sph::CreateScope<sph::OrthographicCameraController>(m_camera);

	m_postFXProjection = glm::ortho(-halfSize.x, halfSize.x, -halfSize.y, halfSize.y, -1.0f, 1.0f);

	// Renderer
	m_renderer2D = sph::CreateScope<sph::Renderer2D>();
	m_renderer2D->Init();
	sph::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });

	// Texture
	m_texture = sph::Texture2D::Create("Background.png");
	m_playerTexture = sph::Texture2D::Create("Player.png");
	
	// Framebuffer
	m_framebuffer = sph::Framebuffer::Create({ (uint32_t)m_app->GetWindow().GetWidth(), (uint32_t)m_app->GetWindow().GetHeight() });

	// AppData buffer
	glm::vec2 data = m_app->GetWindow().GetSize();
	m_appDataUniformBuffer = sph::UniformBuffer::Create({ { sph::ShaderDataType::Float2, "Resolution"} }, 0);
	m_appDataUniformBuffer->SetData(&data, sizeof(glm::vec2));

	// Create Lights
	m_lightData.UploadLightsData(
	{
		{ { 1.0f, 1.0f, 1.0f , 1.0f}, { 100.0f, 100.0f }, 1.f, 0.3f },
		//{ { 1.0f, 1.0f, 1.0f , 1.0f}, { 100.0f, 100.0f }, 1.f, 0.3f },
	});

	// tilemap
	m_tilemapTexture = sph::Texture2D::Create("TileMap.png");

	LoadTileMap();

	// Sprite
	auto texture = sph::Texture2D::Create("Sprite.png");
	m_sprite = sph::Sprite::Create(texture, true);
	m_sprite->SetSize({ 500.0f, 500.0f });
	m_sprite->SetOffset(glm::vec2(0.5f));
}

void TestLighting::OnDetach()
{
	m_renderer2D->Shutdown();
}

void TestLighting::OnUpdate(sph::DeltaTime _dt)
{
	m_cameraController->OnUpdate(_dt);

	glm::vec2 mousePosition = m_camera->ScreenToWorld(sph::Input::GetMousePosition());
	m_sprite->SetPosition(mousePosition);
	//m_lightData.GetUniformBuffer()->SetData(&mousePosition, sizeof(glm::vec2), 32);
}

void TestLighting::OnRender(const sph::Ref<sph::Renderer>& _renderer)
{
	sph::Renderer::Stats::Reset();
	sph::RenderCommand::Clear();

	m_framebuffer->Bind();
	m_framebuffer->Clear();
	_renderer->BeginScene(*m_camera);
	{
		const float tileSize = 48;
		const glm::vec2 mapOffest = { tileSize * (MAP_SIZE_X / 2.0f), tileSize * (MAP_SIZE_Y / 2.0f) };

		for (uint32_t y = 0; y < MAP_SIZE_Y; y++)
		{
			for (uint32_t x = 0; x < MAP_SIZE_X; x++)
			{
				int32_t tileIndex = m_tileMapData[y * MAP_SIZE_X + x];
				if (tileIndex == -1) continue;

				glm::vec3 position = { x * tileSize - mapOffest.x ,1 - y * tileSize + mapOffest.y, 0.0f };
				_renderer->DrawQuad(position, glm::vec2{ tileSize, tileSize }, m_subTexture[tileIndex]);
			}
		}

		_renderer->DrawQuad({ 1280.0f, 0.0f, 0.0f }, m_app->GetWindow().GetSize(), m_texture);
		_renderer->DrawQuad({ 100.0f, 100.0f, 0.0f }, { 100.0f, 100.0f }, m_playerTexture);
		_renderer->DrawSprite(*m_sprite);
	}
	_renderer->EndScene();
	m_framebuffer->Unbind(); 

	m_renderer2D->BeginScene(m_postFXProjection);
	{
		auto& lightShader = m_lightData.GetShader();
		lightShader->Bind();
		lightShader->SetMat4("u_viewProjection", m_postFXProjection);
		lightShader->SetMat4("u_sceneVP", m_camera->GetViewProjectionMatrix());
		lightShader->SetFloat3("u_ambientLight", m_ambiantLightColor);

		m_renderer2D->DrawQuad({ 0.0f, 0.0f, 0.0f }, m_app->GetWindow().GetSize(), m_framebuffer->GetTextureAttachment(), lightShader);
	}
	m_renderer2D->EndScene();
}

void TestLighting::OnImGuiRender()
{
}

void TestLighting::OnEvent(sph::Event& _event)
{
	m_cameraController->OnEvent(_event);
}

void TestLighting::LoadTileMap()
{
	std::ifstream file("Demo.csv");
	if (!file.is_open())
	{
		ASSERT(false, "Failed to open file");
		return;
	}

	std::string line;
	uint32_t row = 0;
	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		std::string cell;
		uint32_t col = 0;
		while (std::getline(ss, cell, ','))
		{
			m_tileMapData[row * MAP_SIZE_X + col] = std::stoi(cell);
			col++;
		}
		row++;
	}
	file.close();

	glm::vec2 cellSize = { 16, 16 };
	glm::vec2 cellNumber = { 22, 6 };

	for (auto value : m_tileMapData)
	{
		if (value == -1) continue;

		glm::vec2 index = { value % (int)cellNumber.x, cellNumber.y - (value / (int)cellNumber.x) };
		m_subTexture[value] = sph::SubTexture2D::Create(m_tilemapTexture, index, cellSize);
	}
}