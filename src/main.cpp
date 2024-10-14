#include <Geode/Geode.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/modify/SliderTouchLogic.hpp>

using namespace geode::prelude;

class $modify(MySliderTouchLogic, SliderTouchLogic) {

	struct Fields {
		std::function<void(float)> m_callback;
	};

	void setCallback(std::function<void(float)> callback) {
		m_fields->m_callback = callback;
	}

    void ccTouchMoved(cocos2d::CCTouch* p0, cocos2d::CCEvent* p1) {
		SliderTouchLogic::ccTouchMoved(p0, p1);
		if (m_fields->m_callback) m_fields->m_callback(m_thumb->getValue());
	}
};

class $modify(MyOptionsLayer, OptionsLayer) {

	struct Fields {
		geode::TextInput* m_musicValueInput;
		geode::TextInput* m_sfxValueInput;
		Slider* m_musicSlider;
		Slider* m_sfxSlider;
	};

	void customSetup() {
		OptionsLayer::customSetup();

		if (Slider* slider = typeinfo_cast<Slider*>(m_mainLayer->getChildByID("music-slider"))) {
			m_fields->m_musicSlider = slider;
		}
		if (Slider* slider = typeinfo_cast<Slider*>(m_mainLayer->getChildByID("sfx-slider"))) {
			m_fields->m_sfxSlider = slider;
		}

		float offset = 48.f;
		
		m_fields->m_musicValueInput = geode::TextInput::create(40, "0");
		m_fields->m_musicValueInput->setFilter("0123456789");
		m_fields->m_musicValueInput->setScale(0.8f);
		m_fields->m_musicValueInput->setID("music-volume-input"_spr);
		m_fields->m_musicValueInput->getBGSprite()->setPositionY(m_fields->m_musicValueInput->getBGSprite()->getPositionY()-1);
		m_fields->m_musicValueInput->setCallback([this] (std::string val) {
			Result<int> res = geode::utils::numFromString<int>(val);
			if (res.isOk()) {
				int percent = res.unwrap();
				if (percent < 0 || percent > 100) {
					percent = std::max(std::min(percent, 100), 0);
				}
				if (m_fields->m_musicSlider) m_fields->m_musicSlider->setValue(percent/100.f);
				FMODAudioEngine::get()->m_musicVolume = percent/100.f;
				musicSliderChanged(m_fields->m_musicSlider->getThumb());
				m_fields->m_musicValueInput->setString(geode::utils::numToString(percent));
			}
			else {
				if (m_fields->m_musicSlider) m_fields->m_musicSlider->setValue(0.f);
				musicSliderChanged(m_fields->m_musicSlider->getThumb());
			}
		});

		m_fields->m_musicValueInput->setString(geode::utils::numToString((int) (m_fields->m_musicSlider->getThumb()->getValue() * 100)));

		m_fields->m_sfxValueInput = geode::TextInput::create(40, "0");
		m_fields->m_sfxValueInput->setFilter("0123456789");
		m_fields->m_sfxValueInput->setScale(0.8f);
		m_fields->m_sfxValueInput->setID("sfx-volume-input"_spr);
		m_fields->m_sfxValueInput->getBGSprite()->setPositionY(m_fields->m_sfxValueInput->getBGSprite()->getPositionY()-1);
		m_fields->m_sfxValueInput->setCallback([this] (std::string val) {
			Result<int> res = geode::utils::numFromString<int>(val);
			if (res.isOk()) {
				int percent = res.unwrap();
				if (percent < 0 || percent > 100) {
					percent = std::max(std::min(percent, 100), 0);
				}
				if (m_fields->m_sfxSlider) m_fields->m_sfxSlider->setValue(percent/100.f);
				sfxSliderChanged(m_fields->m_sfxSlider->getThumb());
				m_fields->m_sfxValueInput->setString(geode::utils::numToString(percent));
			}
			else {
				if (m_fields->m_sfxSlider) m_fields->m_sfxSlider->setValue(0.f);
				sfxSliderChanged(m_fields->m_sfxSlider->getThumb());
			}
		});

		m_fields->m_sfxValueInput->setString(geode::utils::numToString((int) (m_fields->m_sfxSlider->getThumb()->getValue() * 100)));

		m_mainLayer->addChild(m_fields->m_musicValueInput);
		m_mainLayer->addChild(m_fields->m_sfxValueInput);
		
		if (m_fields->m_musicSlider) {
			m_fields->m_musicSlider->setPositionX(m_fields->m_musicSlider->getPositionX() - offset);
			m_fields->m_musicValueInput->setPositionX(m_fields->m_musicSlider->getPositionX() + 131);
			m_fields->m_musicValueInput->setPositionY(m_fields->m_musicSlider->getPositionY() + 2);

			static_cast<MySliderTouchLogic*>(m_fields->m_musicSlider->m_touchLogic)->setCallback([this] (float val) {
				m_fields->m_musicValueInput->setString(geode::utils::numToString((int) (val * 100)));
			});
		}

		if (m_fields->m_sfxSlider) {
			m_fields->m_sfxSlider->setPositionX(m_fields->m_sfxSlider->getPositionX() - offset);
			m_fields->m_sfxValueInput->setPositionX(m_fields->m_sfxSlider->getPositionX() + 131);
			m_fields->m_sfxValueInput->setPositionY(m_fields->m_sfxSlider->getPositionY() + 2);

			static_cast<MySliderTouchLogic*>(m_fields->m_sfxSlider->m_touchLogic)->setCallback([this] (float val) {
				m_fields->m_sfxValueInput->setString(geode::utils::numToString((int) (val * 100)));
			});
		}
		
		CCLabelBMFont* musicPercentLabel = CCLabelBMFont::create("%", "bigFont.fnt");
		musicPercentLabel->setPositionX(m_fields->m_musicValueInput->getPositionX() + 25);
		musicPercentLabel->setPositionY(m_fields->m_musicValueInput->getPositionY());

		musicPercentLabel->setScale(0.5f);
		CCLabelBMFont* sfxPercentLabel = CCLabelBMFont::create("%", "bigFont.fnt");
		sfxPercentLabel->setPositionX(m_fields->m_sfxValueInput->getPositionX() + 25);
		sfxPercentLabel->setPositionY(m_fields->m_sfxValueInput->getPositionY());
		sfxPercentLabel->setScale(0.5f);

		m_mainLayer->addChild(musicPercentLabel);
		m_mainLayer->addChild(sfxPercentLabel);

		if (CCLabelBMFont* musicLabel = typeinfo_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("music-label"))) {
			musicLabel->setPositionX(musicLabel->getPositionX() - offset);
		}
		if (CCLabelBMFont* sfxLabel = typeinfo_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("sfx-label"))) {
			sfxLabel->setPositionX(sfxLabel->getPositionX() - offset);
		}
	}
};