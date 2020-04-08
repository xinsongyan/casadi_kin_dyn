
#define PINOCCHIO_URDFDOM_TYPEDEF_SHARED_PTR
#include <pinocchio/parsers/urdf.hpp>
#include <pinocchio/algorithm/contact-dynamics.hpp>
#include <pinocchio/algorithm/frames.hpp>
#include <pinocchio/algorithm/rnea.hpp>
#include <pinocchio/algorithm/contact-dynamics.hpp>
#include <pinocchio/algorithm/centroidal.hpp>
#include <pinocchio/algorithm/crba.hpp>

#include "PinocchioWrapper.h"

class PinocchioWrapper::Impl
{

public:

    Impl(urdf::ModelInterfaceSharedPtr urdf)
    {
        pinocchio::urdf::buildModel(urdf, _model, false); // verbose
        _data = new pinocchio::Data(_model);
    }

    void update(const Eigen::VectorXd& q)
    {
        pinocchio::computeJointJacobians(_model, data(), q);
        pinocchio::framesForwardKinematics(_model, data(), q);
    }

    Eigen::Affine3d getPose(std::string link)
    {
        auto se3 = data().oMf.at(_model.getFrameId(link));

        Eigen::Affine3d T;
        T.setIdentity();
        T.translation() = se3.translation();
        T.linear() = se3.rotation().derived();

        return T;
    }

    Eigen::MatrixXd getJacobian(std::string link)
    {
        Eigen::Matrix<double, 6, -1> J;
        J.setZero(6, _model.nv);
        pinocchio::getFrameJacobian(_model, data(),
                                    _model.getFrameId(link),
                                    pinocchio::ReferenceFrame::LOCAL_WORLD_ALIGNED,
                                    J);

        return J;
    }

    int nq()
    {
        return _model.nq;
    }

    ~Impl()
    {
        delete _data;
    }

private:

    pinocchio::Data& data()
    {
        return *_data;
    }

    pinocchio::Model _model;
    pinocchio::Data * _data;

};

PinocchioWrapper::PinocchioWrapper(urdf::ModelInterfaceSharedPtr urdf)
{
    impl = new Impl(urdf);
}

void PinocchioWrapper::update(const Eigen::VectorXd& q)
{
    return impl->update(q);
}

Eigen::Affine3d PinocchioWrapper::getPose(std::string link)
{
    return impl->getPose(link);
}

Eigen::MatrixXd PinocchioWrapper::getJacobian(std::string link)
{
    return impl->getJacobian(link);
}

int PinocchioWrapper::nq()
{
    return impl->nq();
}

PinocchioWrapper::~PinocchioWrapper()
{
    delete impl;
}
