#pragma once
#include "../graphics/IRenderer.hpp"

class Penger : public Luna::GameObject {
public:
    Luna::IModel* model;

    explicit Penger(std::shared_ptr<Luna::Configuration> config) :
    model(Luna::IModel::Create(config.get(), "assets/penger-obj-main/suitger/suitedpenger.obj", false))
    {}

    ~Penger() override {
        delete model;
    }

    void Draw(const std::unique_ptr<Luna::IRenderer> &renderer) const override;
};
