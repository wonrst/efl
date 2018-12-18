using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.ComponentModel;

namespace Efl {

public class MonoModel<T> : Efl.MonoModelInternal, IDisposable
{
   ///<summary>Pointer to the native class description.</summary>
   public override System.IntPtr NativeClass {
      get {
         return klass;
      }
   }
   public MonoModel (Efl.Object parent = null, ConstructingMethod init_cb=null) : base(nativeInherit.class_initializer, "MonoModelInternal", Efl.MonoModelInternal.efl_mono_model_internal_class_get(), typeof(MonoModelInternal), parent, ref klass)
   {

     var properties = typeof(T).GetProperties();
     foreach(var prop in properties)
     {
        AddProperty(prop.Name, System.IntPtr.Zero);
     }
         
     if (init_cb != null) {
        init_cb(this);
     }
     FinishInstantiation();
   }
    
   ~MonoModel()
   {
       Dispose(false);
   }
   public void Add (T o)
   {
       // System.IntPtr child = 
       
       // var properties = typeof(T).GetProperties();
       // foreach(var prop in properties)
       // {
           
       // }
   }
   // void Dispose(bool disposing)
   // {
   //    if (handle != System.IntPtr.Zero) {
   //       Efl.Eo.Globals.efl_unref(handle);
   //       handle = System.IntPtr.Zero;
   //    }
   // }
   // public void Dispose()
   // {
   //     Dispose(true);
   //     GC.SuppressFinalize(this);
   // }
}

}
