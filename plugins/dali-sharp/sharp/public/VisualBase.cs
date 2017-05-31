//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.10
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------

namespace Dali {

public class VisualBase : BaseHandle {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;

  internal VisualBase(global::System.IntPtr cPtr, bool cMemoryOwn) : base(NDalicPINVOKE.VisualBase_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(VisualBase obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~VisualBase() {
    DisposeQueue.Instance.Add(this);
  }

  public override void Dispose() {
    if (!Window.IsInstalled()) {
      DisposeQueue.Instance.Add(this);
      return;
    }

    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          NDalicPINVOKE.delete_VisualBase(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
      base.Dispose();
    }
  }


  public VisualBase() : this(NDalicPINVOKE.new_VisualBase__SWIG_0(), true) {
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  public VisualBase(VisualBase handle) : this(NDalicPINVOKE.new_VisualBase__SWIG_1(VisualBase.getCPtr(handle)), true) {
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetName(string name) {
    NDalicPINVOKE.VisualBase_SetName(swigCPtr, name);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  public string GetName() {
    string ret = NDalicPINVOKE.VisualBase_GetName(swigCPtr);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void SetTransformAndSize(Property.Map transform, Vector2 controlSize) {
    NDalicPINVOKE.VisualBase_SetTransformAndSize(swigCPtr, Property.Map.getCPtr(transform), Vector2.getCPtr(controlSize));
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  public float GetHeightForWidth(float width) {
    float ret = NDalicPINVOKE.VisualBase_GetHeightForWidth(swigCPtr, width);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public float GetWidthForHeight(float height) {
    float ret = NDalicPINVOKE.VisualBase_GetWidthForHeight(swigCPtr, height);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void GetNaturalSize(Vector2 naturalSize) {
    NDalicPINVOKE.VisualBase_GetNaturalSize(swigCPtr, Vector2.getCPtr(naturalSize));
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  public void SetDepthIndex(float index) {
    NDalicPINVOKE.VisualBase_SetDepthIndex(swigCPtr, index);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  public float GetDepthIndex() {
    float ret = NDalicPINVOKE.VisualBase_GetDepthIndex(swigCPtr);
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void CreatePropertyMap(Property.Map map) {
    NDalicPINVOKE.VisualBase_CreatePropertyMap(swigCPtr, Property.Map.getCPtr(map));
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

  internal VisualBase(SWIGTYPE_p_Dali__Toolkit__Internal__Visual__Base impl) : this(NDalicPINVOKE.new_VisualBase__SWIG_2(SWIGTYPE_p_Dali__Toolkit__Internal__Visual__Base.getCPtr(impl)), true) {
    if (NDalicPINVOKE.SWIGPendingException.Pending) throw NDalicPINVOKE.SWIGPendingException.Retrieve();
  }

}

}