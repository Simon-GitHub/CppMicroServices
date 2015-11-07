/*=============================================================================

  Library: CppMicroServices

  Copyright (c) The CppMicroServices developers. See the COPYRIGHT
  file at the top-level directory of this distribution and at
  https://github.com/saschazelzer/CppMicroServices/COPYRIGHT .

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/


#ifndef USSERVICETRACKERCUSTOMIZER_H
#define USSERVICETRACKERCUSTOMIZER_H

#include "usServiceReference.h"

namespace us {

///cond
template<class S, class T = S*, class Enable = void>
struct ServiceTrackerCustomizer {
};

namespace detail {

template<class S>
struct ServiceArg
{
  typedef S* type;
};

template<>
struct ServiceArg<void>
{
  typedef const InterfaceMap& type;
};

}
///endcond

/**
 * \ingroup MicroServices
 *
 * The <code>ServiceTrackerCustomizer</code> interface allows a
 * <code>ServiceTracker</code> to customize the service objects that are
 * tracked. A <code>ServiceTrackerCustomizer</code> is called when a service is
 * being added to a <code>ServiceTracker</code>. The
 * <code>ServiceTrackerCustomizer</code> can then return an object for the
 * tracked service. A <code>ServiceTrackerCustomizer</code> is also called when
 * a tracked service is modified or has been removed from a
 * <code>ServiceTracker</code>.
 *
 * <p>
 * The methods in this interface may be called as the result of a
 * <code>ServiceEvent</code> being received by a <code>ServiceTracker</code>.
 * Since <code>ServiceEvent</code>s are synchronously delivered,
 * it is highly recommended that implementations of these methods do
 * not register (<code>BundleContext::RegisterService</code>), modify (
 * <code>ServiceRegistration::SetProperties</code>) or unregister (
 * <code>ServiceRegistration::Unregister</code>) a service while being
 * synchronized on any object.
 *
 * <p>
 * The <code>ServiceTracker</code> class is thread-safe. It does not call a
 * <code>ServiceTrackerCustomizer</code> while holding any locks.
 * <code>ServiceTrackerCustomizer</code> implementations must also be
 * thread-safe.
 *
 * \tparam S The type of the service being tracked
 * \tparam T The type of the tracked object.
 * \remarks This class is thread safe.
 */
template<class S, class T>
struct ServiceTrackerCustomizer<S, T, typename std::enable_if<std::is_pointer<T>::value && !std::is_same<S*,T>::value>::type>
{

  struct TypeTraits {
    typedef S ServiceType;
    typedef T TrackedType;
    typedef T TrackedReturnType;
    typedef T TrackedArgType;
    typedef ServiceReference<ServiceType> ServiceReferenceType;

    static bool IsValid(typename std::remove_pointer<TrackedType>::type const* t)
    {
      return t != nullptr;
    }

    static TrackedReturnType DefaultValue()
    {
      return nullptr;
    }

    static void Dispose(TrackedType& t)
    {
      t = nullptr;
    }

    static TrackedReturnType ConvertToTrackedType(typename detail::ServiceArg<ServiceType>::type)
    {
      throw std::runtime_error("A custom ServiceTrackerCustomizer instance is required for custom tracked objects.");
    }
  };

  typedef typename TypeTraits::ServiceType ServiceType;
  typedef typename TypeTraits::TrackedType TrackedType;
  typedef typename TypeTraits::TrackedReturnType TrackedReturnType;
  typedef typename TypeTraits::TrackedArgType TrackedArgType;
  typedef typename TypeTraits::ServiceReferenceType ServiceReferenceType;

  virtual ~ServiceTrackerCustomizer() {}

  /**
   * A service is being added to the <code>ServiceTracker</code>.
   *
   * <p>
   * This method is called before a service which matched the search
   * parameters of the <code>ServiceTracker</code> is added to the
   * <code>ServiceTracker</code>. This method should return the service object
   * to be tracked for the specified <code>ServiceReference</code>. The
   * returned service object is stored in the <code>ServiceTracker</code> and
   * is available from the <code>GetService</code> and
   * <code>GetServices</code> methods.
   *
   * @param reference The reference to the service being added to the
   *        <code>ServiceTracker</code>.
   * @return The service object to be tracked for the specified referenced
   *         service or <code>0</code> if the specified referenced service
   *         should not be tracked.
   */
  virtual TrackedReturnType AddingService(const ServiceReferenceType& reference) = 0;

  /**
   * A service tracked by the <code>ServiceTracker</code> has been modified.
   *
   * <p>
   * This method is called when a service being tracked by the
   * <code>ServiceTracker</code> has had it properties modified.
   *
   * @param reference The reference to the service that has been modified.
   * @param service The service object for the specified referenced service.
   */
  virtual void ModifiedService(const ServiceReferenceType& reference, TrackedArgType service) = 0;

  /**
   * A service tracked by the <code>ServiceTracker</code> has been removed.
   *
   * <p>
   * This method is called after a service is no longer being tracked by the
   * <code>ServiceTracker</code>.
   *
   * @param reference The reference to the service that has been removed.
   * @param service The service object for the specified referenced service.
   */
  virtual void RemovedService(const ServiceReferenceType& reference, TrackedArgType service) = 0;
};

template<class S>
struct ServiceTrackerCustomizer<S, S*, void>
{

  struct TypeTraits {
    typedef S ServiceType;
    typedef S* TrackedType;
    typedef S* TrackedReturnType;
    typedef S* TrackedArgType;
    typedef ServiceReference<ServiceType> ServiceReferenceType;

    static bool IsValid(const TrackedType t)
    {
      return t != nullptr;
    }

    static TrackedReturnType DefaultValue()
    {
      return nullptr;
    }

    static void Dispose(TrackedType& t)
    {
      t = nullptr;
    }

    static TrackedReturnType ConvertToTrackedType(TrackedReturnType s)
    {
      return s;
    }
  };

  typedef typename TypeTraits::ServiceType ServiceType;
  typedef typename TypeTraits::TrackedType TrackedType;
  typedef typename TypeTraits::TrackedReturnType TrackedReturnType;
  typedef typename TypeTraits::TrackedArgType TrackedArgType;
  typedef typename TypeTraits::ServiceReferenceType ServiceReferenceType;

  virtual ~ServiceTrackerCustomizer() {}
  virtual TrackedReturnType AddingService(const ServiceReferenceType& reference) = 0;
  virtual void ModifiedService(const ServiceReferenceType& reference, TrackedArgType service) = 0;
  virtual void RemovedService(const ServiceReferenceType& reference, TrackedArgType service) = 0;
};

template<class S, class T>
struct ServiceTrackerCustomizer<S, T, typename std::enable_if<!std::is_pointer<T>::value && std::is_constructible<T>::value &&
    (std::is_convertible<T,bool>::value || std::is_constructible<bool,T>::value)>::type>
{

  struct TypeTraits {
    typedef S ServiceType;
    typedef T TrackedType;
    typedef T TrackedReturnType;
    typedef T& TrackedArgType;
    typedef ServiceReference<ServiceType> ServiceReferenceType;

    static bool IsValid(const TrackedType& t)
    {
      return static_cast<bool>(t);
    }

    static TrackedReturnType DefaultValue()
    {
      return TrackedReturnType();
    }

    static void Dispose(TrackedArgType)
    {
    }

    static TrackedReturnType ConvertToTrackedType(typename detail::ServiceArg<ServiceType>::type)
    {
      throw std::runtime_error("A custom ServiceTrackerCustomizer instance is required for custom tracked objects.");
    }
  };

  typedef typename TypeTraits::ServiceType ServiceType;
  typedef typename TypeTraits::TrackedType TrackedType;
  typedef typename TypeTraits::TrackedReturnType TrackedReturnType;
  typedef typename TypeTraits::TrackedArgType TrackedArgType;
  typedef typename TypeTraits::ServiceReferenceType ServiceReferenceType;

  virtual ~ServiceTrackerCustomizer() {}
  virtual TrackedReturnType AddingService(const ServiceReferenceType& reference) = 0;
  virtual void ModifiedService(const ServiceReferenceType& reference, TrackedArgType service) = 0;
  virtual void RemovedService(const ServiceReferenceType& reference, TrackedArgType service) = 0;
};

template<>
struct ServiceTrackerCustomizer<void, void*, void>
{

  struct TypeTraits {
    typedef void ServiceType;
    typedef void* TrackedType;
    typedef InterfaceMap TrackedReturnType;
    typedef const InterfaceMap& TrackedArgType;
    typedef ServiceReference<ServiceType> ServiceReferenceType;

    static bool IsValid(TrackedArgType t)
    {
      return !t.empty();
    }

    static TrackedReturnType DefaultValue()
    {
      return TrackedReturnType();
    }

    static void Dispose(InterfaceMap& t)
    {
      t.clear();
    }

    static TrackedReturnType ConvertToTrackedType(TrackedArgType im)
    {
      return im;
    }
  };

  typedef TypeTraits::ServiceType ServiceType;
  typedef TypeTraits::TrackedType TrackedType;
  typedef TypeTraits::TrackedReturnType TrackedReturnType;
  typedef TypeTraits::TrackedArgType TrackedArgType;
  typedef TypeTraits::ServiceReferenceType ServiceReferenceType;

  virtual ~ServiceTrackerCustomizer() {}
  virtual TrackedReturnType AddingService(const ServiceReferenceType& reference) = 0;
  virtual void ModifiedService(const ServiceReferenceType& reference, TrackedArgType service) = 0;
  virtual void RemovedService(const ServiceReferenceType& reference, TrackedArgType service) = 0;
};

}

#endif // USSERVICETRACKERCUSTOMIZER_H
